#include "Characters/EnemyCharacter.h"

#include "AIController.h"
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

	bCanAttack = true;
	bIsDead = false;
	TargetPlayer = nullptr;

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(HealthComponent))
	{
		HealthComponent->OnDeath.AddDynamic(this, &AEnemyCharacter::HandleDeath);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("EnemyCharacter BeginPlay failed: HealthComponent is invalid."));
	}

	OnTakeAnyDamage.AddDynamic(this, &AEnemyCharacter::HandleAnyDamage);

	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();

	if (IsValid(MovementComponent))
	{
		MovementComponent->MaxWalkSpeed = ChaseSpeed;
	}

	FindTargetPlayer();
}

void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsDead)
	{
		return;
	}

	UpdateEnemyBehavior();
}

UHealthComponent* AEnemyCharacter::GetHealthComponent() const
{
	return HealthComponent;
}

void AEnemyCharacter::FindTargetPlayer()
{
	TargetPlayer = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));

	if (!IsValid(TargetPlayer))
	{
		UE_LOG(LogTemp, Warning, TEXT("EnemyCharacter could not find PlayerCharacter."));
	}
}

void AEnemyCharacter::UpdateEnemyBehavior()
{
	if (!IsValid(TargetPlayer))
	{
		FindTargetPlayer();
		return;
	}

	const float DistanceToTarget = GetDistanceToTarget();

	if (DistanceToTarget > DetectionRange)
	{
		return;
	}

	if (DistanceToTarget <= AttackRange)
	{
		TryAttackTarget();
		return;
	}

	ChaseTarget();
}

void AEnemyCharacter::ChaseTarget()
{
	if (!IsValid(TargetPlayer))
	{
		return;
	}

	AAIController* AIController = Cast<AAIController>(GetController());

	if (!IsValid(AIController))
	{
		UE_LOG(LogTemp, Warning, TEXT("EnemyCharacter has no valid AIController."));
		return;
	}

	AIController->MoveToActor(TargetPlayer, AttackRange * 0.8f);
}

void AEnemyCharacter::TryAttackTarget()
{
	if (!bCanAttack || !IsValid(TargetPlayer))
	{
		return;
	}

	bCanAttack = false;

	UGameplayStatics::ApplyDamage(
		TargetPlayer,
		AttackDamage,
		GetController(),
		this,
		nullptr
	);

	UE_LOG(LogTemp, Warning, TEXT("Enemy attacked player. Damage: %f"), AttackDamage);

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

float AEnemyCharacter::GetDistanceToTarget() const
{
	if (!IsValid(TargetPlayer))
	{
		return TNumericLimits<float>::Max();
	}

	return FVector::Dist(GetActorLocation(), TargetPlayer->GetActorLocation());
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

	GetWorldTimerManager().ClearTimer(AttackTimerHandle);

	AAIController* AIController = Cast<AAIController>(GetController());

	if (IsValid(AIController))
	{
		AIController->StopMovement();
	}

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

	UE_LOG(LogTemp, Warning, TEXT("Enemy died."));
}