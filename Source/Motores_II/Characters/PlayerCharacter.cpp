#include "Characters/PlayerCharacter.h"

#include "AbilitySystemComponent.h"
#include "Components/HealthComponent.h"
#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameModes/GameplayGameMode.h"
#include "GameplayAbilitySpec.h"
#include "InputAction.h"
#include "Kismet/GameplayStatics.h"
#include "Motores_II/Projectiles/PlayerProjectile.h"
#include "TimerManager.h"

APlayerCharacter::APlayerCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	InitializeAbilitySystem();
	GiveStartupAbilities();

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

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);

	if (!IsValid(EnhancedInputComponent))
	{
		UE_LOG(LogTemp, Error, TEXT("Player input failed: InputComponent is not EnhancedInputComponent."));
		return;
	}

	if (!IsValid(AttackAction))
	{
		UE_LOG(LogTemp, Error, TEXT("Player input failed: AttackAction is not assigned."));
		return;
	}

	EnhancedInputComponent->BindAction(
		AttackAction,
		ETriggerEvent::Started,
		this,
		&APlayerCharacter::AttackInputStarted
	);

	UE_LOG(LogTemp, Warning, TEXT("PlayerCharacter bound AttackAction: %s"), *AttackAction->GetName());
}

void APlayerCharacter::AttackInputStarted()
{
	UE_LOG(LogTemp, Warning, TEXT("Attack input received."));
	Attack();
}

UAbilitySystemComponent* APlayerCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void APlayerCharacter::RequestShoot()
{
	Attack();
}

UHealthComponent* APlayerCharacter::GetHealthComponent() const
{
	return HealthComponent;
}

void APlayerCharacter::InitializeAbilitySystem()
{
	if (!IsValid(AbilitySystemComponent))
	{
		UE_LOG(LogTemp, Error, TEXT("InitializeAbilitySystem failed: AbilitySystemComponent is invalid."));
		return;
	}

	AbilitySystemComponent->InitAbilityActorInfo(this, this);
}

void APlayerCharacter::GiveStartupAbilities()
{
	if (!IsValid(AbilitySystemComponent))
	{
		UE_LOG(LogTemp, Error, TEXT("GiveStartupAbilities failed: AbilitySystemComponent is invalid."));
		return;
	}

	if (!IsValid(ShootAbilityClass))
	{
		UE_LOG(LogTemp, Error, TEXT("GiveStartupAbilities failed: ShootAbilityClass is not assigned."));
		return;
	}

	AbilitySystemComponent->GiveAbility(
		FGameplayAbilitySpec(ShootAbilityClass, 1, INDEX_NONE, this)
	);

	UE_LOG(LogTemp, Warning, TEXT("GA_Shoot granted successfully: %s"), *ShootAbilityClass->GetName());
}

void APlayerCharacter::HandleDeath()
{
	GetWorldTimerManager().ClearTimer(SpeedBoostTimerHandle);
	GetWorldTimerManager().ClearTimer(JumpBoostTimerHandle);

	if (IsValid(AbilitySystemComponent))
	{
		AbilitySystemComponent->CancelAllAbilities();
	}

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
	UE_LOG(LogTemp, Warning, TEXT("PlayerCharacter::Attack called."));

	if (!IsValid(AbilitySystemComponent))
	{
		UE_LOG(LogTemp, Error, TEXT("Attack failed: AbilitySystemComponent is invalid."));
		return;
	}

	if (!IsValid(ShootAbilityClass))
	{
		UE_LOG(LogTemp, Error, TEXT("Attack failed: ShootAbilityClass is not assigned."));
		return;
	}

	const bool bActivated = AbilitySystemComponent->TryActivateAbilityByClass(ShootAbilityClass);

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("TryActivateAbilityByClass result: %s"),
		bActivated ? TEXT("true") : TEXT("false")
	);

	if (!bActivated)
	{
		UE_LOG(LogTemp, Warning, TEXT("GA_Shoot was not activated. It may be on cooldown or blocked by GAS."));
	}
}

bool APlayerCharacter::SpawnProjectileFromAbility()
{
	UE_LOG(LogTemp, Warning, TEXT("SpawnProjectileFromAbility called."));
	if (!IsValid(ProjectileClass))
	{
		UE_LOG(LogTemp, Error, TEXT("SpawnProjectileFromAbility failed: ProjectileClass is not assigned."));
		return false;
	}

	UWorld* World = GetWorld();

	if (!IsValid(World))
	{
		return false;
	}

	const FVector AttackDirection = GetAttackDirection();

	if (AttackDirection.IsNearlyZero())
	{
		return false;
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
		UE_LOG(LogTemp, Error, TEXT("SpawnProjectileFromAbility failed: Projectile spawn returned null."));
		return false;
	}

	Projectile->InitializeProjectile(AttackDirection);

	return true;
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
