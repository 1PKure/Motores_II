#include "Characters/PlayerCharacter.h"

#include "Characters/EnemyCharacter.h"
#include "Components/HealthComponent.h"
#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameModes/GameplayGameMode.h"
#include "InputAction.h"
#include "Kismet/GameplayStatics.h"
#include "Motores_II/Projectiles/PlayerProjectile.h"
#include "TimerManager.h"

APlayerCharacter::APlayerCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(HealthComponent))
	{
		HealthComponent->OnDeath.AddDynamic(this, &APlayerCharacter::HandleDeath);
	}

	OnTakeAnyDamage.AddDynamic(this, &APlayerCharacter::HandleAnyDamage);

	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();

	if (IsValid(MovementComponent))
	{
		DefaultWalkSpeed = MovementComponent->MaxWalkSpeed;
		DefaultJumpZVelocity = MovementComponent->JumpZVelocity;
	}
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (!IsValid(PlayerInputComponent))
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerInputComponent is invalid."));
		return;
	}

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);

	if (!IsValid(EnhancedInputComponent))
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerInputComponent is not an EnhancedInputComponent."));
		return;
	}

	if (!IsValid(AttackAction))
	{
		UE_LOG(LogTemp, Error, TEXT("AttackAction is not assigned."));
		return;
	}

	EnhancedInputComponent->BindAction(
		AttackAction,
		ETriggerEvent::Started,
		this,
		&APlayerCharacter::AttackInputStarted
	);
}

UHealthComponent* APlayerCharacter::GetHealthComponent() const
{
	return HealthComponent;
}

void APlayerCharacter::HandleDeath()
{
	GetWorldTimerManager().ClearTimer(SpeedBoostTimerHandle);
	GetWorldTimerManager().ClearTimer(JumpBoostTimerHandle);
	GetWorldTimerManager().ClearTimer(AttackTimerHandle);

	AGameplayGameMode* GameplayGameMode = Cast<AGameplayGameMode>(UGameplayStatics::GetGameMode(this));

	if (!IsValid(GameplayGameMode))
	{
		UE_LOG(LogTemp, Error, TEXT("HandleDeath failed: GameplayGameMode is invalid."));
		return;
	}

	GameplayGameMode->HandleDefeat();
}

void APlayerCharacter::ApplySpeedBoost(float SpeedMultiplier, float Duration)
{
	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();

	if (!IsValid(MovementComponent))
	{
		return;
	}

	if (DefaultWalkSpeed <= 0.0f)
	{
		DefaultWalkSpeed = MovementComponent->MaxWalkSpeed;
	}

	MovementComponent->MaxWalkSpeed = DefaultWalkSpeed * SpeedMultiplier;

	GetWorldTimerManager().ClearTimer(SpeedBoostTimerHandle);
	GetWorldTimerManager().SetTimer(
		SpeedBoostTimerHandle,
		this,
		&APlayerCharacter::ResetSpeedBoost,
		Duration,
		false
	);
}

void APlayerCharacter::ResetSpeedBoost()
{
	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();

	if (!IsValid(MovementComponent))
	{
		return;
	}

	MovementComponent->MaxWalkSpeed = DefaultWalkSpeed;
}

void APlayerCharacter::ApplyJumpBoost(float JumpMultiplier, float Duration)
{
	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();

	if (!IsValid(MovementComponent))
	{
		return;
	}

	if (DefaultJumpZVelocity <= 0.0f)
	{
		DefaultJumpZVelocity = MovementComponent->JumpZVelocity;
	}

	MovementComponent->JumpZVelocity = DefaultJumpZVelocity * JumpMultiplier;

	GetWorldTimerManager().ClearTimer(JumpBoostTimerHandle);
	GetWorldTimerManager().SetTimer(
		JumpBoostTimerHandle,
		this,
		&APlayerCharacter::ResetJumpBoost,
		Duration,
		false
	);
}

void APlayerCharacter::ResetJumpBoost()
{
	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();

	if (!IsValid(MovementComponent))
	{
		return;
	}

	MovementComponent->JumpZVelocity = DefaultJumpZVelocity;
}

void APlayerCharacter::HandleAnyDamage(
	AActor* DamagedActor,
	float Damage,
	const UDamageType* DamageType,
	AController* InstigatedBy,
	AActor* DamageCauser
)
{
	if (Damage <= 0.0f)
	{
		return;
	}

	if (!IsValid(HealthComponent))
	{
		UE_LOG(LogTemp, Error, TEXT("HandleAnyDamage failed: HealthComponent is invalid."));
		return;
	}

	HealthComponent->ApplyDamage(Damage);
}

void APlayerCharacter::Attack()
{
	if (!bCanAttack)
	{
		return;
	}

	if (!IsValid(ProjectileClass))
	{
		UE_LOG(LogTemp, Error, TEXT("Attack failed: ProjectileClass is not assigned."));
		return;
	}

	UWorld* World = GetWorld();

	if (!IsValid(World))
	{
		return;
	}

	const FVector AttackDirection = GetAttackDirection();

	if (AttackDirection.IsNearlyZero())
	{
		return;
	}

	const FVector SpawnLocation =
		GetActorLocation()
		+ AttackDirection * ProjectileSpawnOffset.X
		+ FVector(0.0f, 0.0f, ProjectileSpawnOffset.Z);

	const FRotator SpawnRotation = AttackDirection.Rotation();

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = this;
	SpawnParameters.Instigator = this;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	APlayerProjectile* Projectile = World->SpawnActor<APlayerProjectile>(
		ProjectileClass,
		SpawnLocation,
		SpawnRotation,
		SpawnParameters
	);

	if (!IsValid(Projectile))
	{
		UE_LOG(LogTemp, Error, TEXT("Attack failed: Projectile spawn returned null."));
		return;
	}

	Projectile->InitializeProjectile(AttackDirection);

	bCanAttack = false;

	World->GetTimerManager().SetTimer(
		AttackTimerHandle,
		this,
		&APlayerCharacter::ResetAttack,
		AttackCooldown,
		false
	);
}

void APlayerCharacter::ResetAttack()
{
	bCanAttack = true;
}

FVector APlayerCharacter::GetAttackDirection() const
{
	const FVector ForwardVector = GetActorForwardVector();

	if (FMath::Abs(ForwardVector.X) >= FMath::Abs(ForwardVector.Y))
	{
		return ForwardVector.X >= 0.0f ? FVector::ForwardVector : -FVector::ForwardVector;
	}

	return ForwardVector.Y >= 0.0f ? FVector::RightVector : -FVector::RightVector;
}

void APlayerCharacter::AttackInputStarted()
{
	Attack();
}