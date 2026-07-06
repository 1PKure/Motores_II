#include "Characters/EnemyCharacter.h"

#include "Characters/PlayerCharacter.h"
#include "Components/HealthComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameModes/GameplayGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

AEnemyCharacter::AEnemyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	bUseControllerRotationYaw = false;

	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();

	if (IsValid(MovementComponent))
	{
		MovementComponent->bOrientRotationToMovement = false;
		MovementComponent->RotationRate = FRotator(0.0f, 720.0f, 0.0f);
		MovementComponent->MaxWalkSpeed = PatrolSpeed;

		MovementComponent->bConstrainToPlane = false;
		MovementComponent->bSnapToPlaneAtStart = false;
	}
}

void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(HealthComponent))
	{
		HealthComponent->OnDeath.AddDynamic(this, &AEnemyCharacter::HandleDeath);
	}

	OnTakeAnyDamage.AddDynamic(this, &AEnemyCharacter::HandleAnyDamage);

	LockedDepthY = GetActorLocation().Y;

	FindTargetPlayer();

	SetEnemyState(EEnemyState::Patrol, true);
}

void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsDead)
	{
		return;
	}

	UpdateEnemyBehavior();

	if (bLockDepthAxis)
	{
		LockDepthAxisPosition();
	}
}

UHealthComponent* AEnemyCharacter::GetHealthComponent() const
{
	return HealthComponent;
}

EEnemyState AEnemyCharacter::GetCurrentState() const
{
	return CurrentState;
}

void AEnemyCharacter::FindTargetPlayer()
{
	TargetPlayer = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
}

void AEnemyCharacter::UpdateEnemyBehavior()
{
	if (!IsValid(TargetPlayer))
	{
		FindTargetPlayer();
	}

	switch (CurrentState)
	{
	case EEnemyState::Patrol:
		HandlePatrolState();
		break;

	case EEnemyState::Chase:
		HandleChaseState();
		break;

	case EEnemyState::Attack:
		HandleAttackState();
		break;

	case EEnemyState::Dead:
		HandleDeadState();
		break;

	default:
		break;
	}
}

void AEnemyCharacter::SetEnemyState(EEnemyState NewState, bool bForce)
{
	if (!bForce && CurrentState == NewState)
	{
		return;
	}

	CurrentState = NewState;

	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();

	if (!IsValid(MovementComponent))
	{
		return;
	}

	switch (CurrentState)
	{
	case EEnemyState::Patrol:
		MovementComponent->SetMovementMode(MOVE_Walking);
		MovementComponent->MaxWalkSpeed = PatrolSpeed;
		MovementComponent->bOrientRotationToMovement = false;
		StopMovement();
		break;

	case EEnemyState::Chase:
		MovementComponent->SetMovementMode(MOVE_Walking);
		MovementComponent->MaxWalkSpeed = ChaseSpeed;
		MovementComponent->bOrientRotationToMovement = false;
		break;

	case EEnemyState::Attack:
		MovementComponent->SetMovementMode(MOVE_Walking);
		MovementComponent->bOrientRotationToMovement = false;
		StopMovement();
		break;

	case EEnemyState::Dead:
		StopMovement();
		MovementComponent->DisableMovement();
		break;

	default:
		break;
	}
}

void AEnemyCharacter::HandlePatrolState()
{
	if (IsTargetValidForDetection())
	{
		SetEnemyState(EEnemyState::Chase);
		return;
	}

	MoveToCurrentPatrolPoint();
}

void AEnemyCharacter::HandleChaseState()
{
	if (!IsTargetInsideLoseSightRange() || !IsTargetInsideAllowedHeight())
	{
		SetEnemyState(EEnemyState::Patrol);
		return;
	}

	if (IsTargetInsideAttackRange())
	{
		SetEnemyState(EEnemyState::Attack);
		return;
	}

	ChaseTarget();
}


void AEnemyCharacter::HandleAttackState()
{
	if (!IsTargetInsideLoseSightRange() || !IsTargetInsideAllowedHeight())
	{
		SetEnemyState(EEnemyState::Patrol);
		return;
	}

	if (!IsTargetInsideAttackRange())
	{
		SetEnemyState(EEnemyState::Chase);
		return;
	}

	StopMovement();
	FaceTarget();
	TryAttackTarget();
}

void AEnemyCharacter::HandleDeadState()
{
	StopMovement();
}

void AEnemyCharacter::MoveToCurrentPatrolPoint()
{
	if (PatrolPoints.Num() == 0)
	{
		StopMovement();
		return;
	}

	if (!PatrolPoints.IsValidIndex(CurrentPatrolPointIndex))
	{
		CurrentPatrolPointIndex = 0;
	}

	AActor* CurrentPatrolPoint = PatrolPoints[CurrentPatrolPointIndex];

	if (!IsValid(CurrentPatrolPoint))
	{
		SelectNextPatrolPoint();
		return;
	}

	const float SwitchDistance = FMath::Max(10.0f, FMath::Min(PatrolSwitchDistance, PatrolAcceptanceRadius));
	const float DistanceToPatrolPoint = GetHorizontalDistanceToLocation(CurrentPatrolPoint->GetActorLocation());

	if (DistanceToPatrolPoint <= SwitchDistance)
	{
		SelectNextPatrolPoint();

		if (!PatrolPoints.IsValidIndex(CurrentPatrolPointIndex))
		{
			return;
		}

		CurrentPatrolPoint = PatrolPoints[CurrentPatrolPointIndex];

		if (!IsValid(CurrentPatrolPoint))
		{
			return;
		}
	}

	MoveAlongX(CurrentPatrolPoint->GetActorLocation().X, SwitchDistance);
}

void AEnemyCharacter::SelectNextPatrolPoint()
{
	if (PatrolPoints.Num() == 0)
	{
		return;
	}

	CurrentPatrolPointIndex = (CurrentPatrolPointIndex + 1) % PatrolPoints.Num();
}

void AEnemyCharacter::ChaseTarget()
{
	if (!IsValid(TargetPlayer))
	{
		return;
	}

	MoveAlongX(TargetPlayer->GetActorLocation().X, ChaseAcceptanceRadius);
}

void AEnemyCharacter::MoveAlongX(float TargetX, float AcceptanceDistance)
{
	const float DeltaX = TargetX - GetActorLocation().X;

	if (FMath::Abs(DeltaX) <= AcceptanceDistance)
	{
		StopMovement();
		return;
	}

	const float DirectionX = DeltaX > 0.0f ? 1.0f : -1.0f;

	AddMovementInput(FVector(DirectionX, 0.0f, 0.0f), 1.0f);

	const float TargetYaw = DirectionX > 0.0f ? 0.0f : 180.0f;
	SetActorRotation(FRotator(0.0f, TargetYaw, 0.0f));
}

void AEnemyCharacter::FaceTarget()
{
	if (!IsValid(TargetPlayer))
	{
		return;
	}

	const float DirectionX = TargetPlayer->GetActorLocation().X - GetActorLocation().X;

	if (FMath::IsNearlyZero(DirectionX))
	{
		return;
	}

	const float TargetYaw = DirectionX > 0.0f ? 0.0f : 180.0f;
	SetActorRotation(FRotator(0.0f, TargetYaw, 0.0f));
}

void AEnemyCharacter::StopMovement()
{
	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();

	if (IsValid(MovementComponent))
	{
		MovementComponent->StopMovementImmediately();
	}
}

void AEnemyCharacter::TryAttackTarget()
{
	if (!bCanAttack || !IsValid(TargetPlayer))
	{
		return;
	}

	if (!IsTargetInsideAttackRange() || !IsTargetInsideAllowedHeight())
	{
		return;
	}

	bCanAttack = false;

	if (IsValid(AttackMontage))
	{
		PlayAnimMontage(AttackMontage);
	}

	UGameplayStatics::ApplyDamage(
		TargetPlayer,
		AttackDamage,
		GetController(),
		this,
		nullptr
	);

	GetWorldTimerManager().SetTimer(
		AttackTimerHandle,
		this,
		&AEnemyCharacter::ResetAttack,
		AttackCooldown,
		false
	);
}

void AEnemyCharacter::ResetAttack()
{
	bCanAttack = true;
}

float AEnemyCharacter::GetHorizontalDistanceToTarget() const
{
	if (!IsValid(TargetPlayer))
	{
		return TNumericLimits<float>::Max();
	}

	return FMath::Abs(TargetPlayer->GetActorLocation().X - GetActorLocation().X);
}

float AEnemyCharacter::GetHorizontalDistanceToLocation(const FVector& Location) const
{
	return FMath::Abs(Location.X - GetActorLocation().X);
}

float AEnemyCharacter::GetHeightDifferenceToTarget() const
{
	if (!IsValid(TargetPlayer))
	{
		return TNumericLimits<float>::Max();
	}

	return FMath::Abs(TargetPlayer->GetActorLocation().Z - GetActorLocation().Z);
}

bool AEnemyCharacter::IsTargetValidForDetection() const
{
	return IsValid(TargetPlayer)
		&& IsTargetInsideDetectionRange()
		&& IsTargetInsideAllowedHeight();
}

bool AEnemyCharacter::IsTargetInsideDetectionRange() const
{
	return GetHorizontalDistanceToTarget() <= DetectionRange;
}

bool AEnemyCharacter::IsTargetInsideLoseSightRange() const
{
	return GetHorizontalDistanceToTarget() <= LoseSightRange;
}

bool AEnemyCharacter::IsTargetInsideAttackRange() const
{
	return GetHorizontalDistanceToTarget() <= AttackRange;
}

bool AEnemyCharacter::IsTargetInsideAllowedHeight() const
{
	return GetHeightDifferenceToTarget() <= MaxChaseHeightDifference;
}

void AEnemyCharacter::LockDepthAxisPosition()
{
	FVector CurrentLocation = GetActorLocation();

	if (FMath::IsNearlyEqual(CurrentLocation.Y, LockedDepthY, 1.0f))
	{
		return;
	}

	CurrentLocation.Y = LockedDepthY;

	SetActorLocation(CurrentLocation, false);
}

void AEnemyCharacter::HandleAnyDamage(
	AActor* DamagedActor,
	float Damage,
	const UDamageType* DamageType,
	AController* InstigatedBy,
	AActor* DamageCauser
)
{
	if (Damage <= 0.0f || bIsDead)
	{
		return;
	}

	if (!IsValid(HealthComponent))
	{
		UE_LOG(LogTemp, Error, TEXT("Enemy HandleAnyDamage failed: HealthComponent is invalid."));
		return;
	}

	HealthComponent->ApplyDamage(Damage);
}

void AEnemyCharacter::HandleDeath()
{
	if (bIsDead)
	{
		return;
	}

	bIsDead = true;

	SetEnemyState(EEnemyState::Dead);

	GetWorldTimerManager().ClearTimer(AttackTimerHandle);

	AGameplayGameMode* GameplayGameMode = Cast<AGameplayGameMode>(UGameplayStatics::GetGameMode(this));

	if (IsValid(GameplayGameMode))
	{
		GameplayGameMode->NotifyEnemyKilled();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Enemy death failed: GameplayGameMode is invalid."));
	}

	SetLifeSpan(2.0f);
}