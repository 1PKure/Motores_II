#include "Characters/PlayerCharacter.h"
#include "Components/HealthComponent.h"
#include "Components/InputComponent.h"
#include "InputCoreTypes.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameModes/GameplayGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Motores_II/Projectiles/PlayerProjectile.h"
#include "EnhancedInputComponent.h"
#include "InputAction.h"
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

	if (HealthComponent)
	{
		HealthComponent->OnDeath.AddDynamic(this, &APlayerCharacter::HandleDeath);
	}
	
	OnTakeAnyDamage.AddDynamic(this, &APlayerCharacter::HandleAnyDamage);
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UE_LOG(LogTemp, Warning, TEXT("[ATTACK DEBUG] SetupPlayerInputComponent called. Character: %s"), *GetName());

	if (!IsValid(PlayerInputComponent))
	{
		UE_LOG(LogTemp, Error, TEXT("[ATTACK DEBUG] PlayerInputComponent is invalid."));
		return;
	}

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		UE_LOG(LogTemp, Warning, TEXT("[ATTACK DEBUG] PlayerInputComponent is EnhancedInputComponent."));

		if (IsValid(AttackAction))
		{
			UE_LOG(LogTemp, Warning, TEXT("[ATTACK DEBUG] Binding AttackAction: %s"), *AttackAction->GetName());

			EnhancedInputComponent->BindAction(
				AttackAction,
				ETriggerEvent::Started,
				this,
				&APlayerCharacter::AttackInputStarted
			);

			EnhancedInputComponent->BindAction(
				AttackAction,
				ETriggerEvent::Triggered,
				this,
				&APlayerCharacter::AttackInputTriggered
			);

			EnhancedInputComponent->BindAction(
				AttackAction,
				ETriggerEvent::Completed,
				this,
				&APlayerCharacter::AttackInputCompleted
			);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("[ATTACK DEBUG] AttackAction is NOT assigned."));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[ATTACK DEBUG] PlayerInputComponent is NOT EnhancedInputComponent."));
	}

	PlayerInputComponent->BindKey(EKeys::H, IE_Pressed, this, &APlayerCharacter::TestDamage);

	
	PlayerInputComponent->BindKey(EKeys::LeftMouseButton, IE_Pressed, this, &APlayerCharacter::DebugDirectMouseAttack);
	PlayerInputComponent->BindKey(EKeys::X, IE_Pressed, this, &APlayerCharacter::DebugDirectXAttack);
}

void APlayerCharacter::TestDamage()
{
	if (!HealthComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("HealthComponent is null."));
		return;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Test damage applied."));
	HealthComponent->ApplyDamage(20.0f);
}

UHealthComponent* APlayerCharacter::GetHealthComponent() const
{
	return HealthComponent;
}

void APlayerCharacter::HandleDeath()
{
	UE_LOG(LogTemp, Warning, TEXT("Player died."));

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
	UE_LOG(LogTemp, Warning, TEXT("[ATTACK DEBUG] Attack() called. Character: %s"), *GetName());

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("[ATTACK DEBUG] bCanAttack: %s"),
		bCanAttack ? TEXT("true") : TEXT("false")
	);

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("[ATTACK DEBUG] ProjectileClass: %s"),
		IsValid(ProjectileClass) ? *ProjectileClass->GetName() : TEXT("NULL")
	);

	if (!bCanAttack)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ATTACK DEBUG] Attack blocked: cooldown active."));
		return;
	}

	if (!IsValid(ProjectileClass))
	{
		UE_LOG(LogTemp, Error, TEXT("[ATTACK DEBUG] Attack failed: ProjectileClass is NULL."));
		return;
	}

	UWorld* World = GetWorld();

	if (!IsValid(World))
	{
		UE_LOG(LogTemp, Error, TEXT("[ATTACK DEBUG] Attack failed: World is invalid."));
		return;
	}

	const FVector AttackDirection = GetAttackDirection();

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("[ATTACK DEBUG] AttackDirection: %s"),
		*AttackDirection.ToString()
	);

	if (AttackDirection.IsNearlyZero())
	{
		UE_LOG(LogTemp, Warning, TEXT("[ATTACK DEBUG] Attack failed: AttackDirection is nearly zero."));
		return;
	}

	const FVector SpawnLocation =
		GetActorLocation()
		+ AttackDirection * ProjectileSpawnOffset.X
		+ FVector(0.0f, 0.0f, ProjectileSpawnOffset.Z);

	const FRotator SpawnRotation = AttackDirection.Rotation();

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("[ATTACK DEBUG] Spawning projectile. Location: %s | Rotation: %s | Class: %s"),
		*SpawnLocation.ToString(),
		*SpawnRotation.ToString(),
		*ProjectileClass->GetName()
	);

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
		UE_LOG(LogTemp, Error, TEXT("[ATTACK DEBUG] SpawnActor returned NULL."));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("[ATTACK DEBUG] Projectile spawned successfully: %s"), *Projectile->GetName());

	Projectile->InitializeProjectile(AttackDirection);

	bCanAttack = false;

	World->GetTimerManager().SetTimer(
		AttackTimerHandle,
		this,
		&APlayerCharacter::ResetAttack,
		AttackCooldown,
		false
	);

	UE_LOG(LogTemp, Warning, TEXT("[ATTACK DEBUG] Attack finished correctly."));
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
	UE_LOG(LogTemp, Warning, TEXT("[ATTACK DEBUG] IA_Attack Started received."));
	Attack();
}

void APlayerCharacter::AttackInputTriggered()
{
	UE_LOG(LogTemp, Warning, TEXT("[ATTACK DEBUG] IA_Attack Triggered received."));
}

void APlayerCharacter::AttackInputCompleted()
{
	UE_LOG(LogTemp, Warning, TEXT("[ATTACK DEBUG] IA_Attack Completed received."));
}

void APlayerCharacter::DebugDirectMouseAttack()
{
	UE_LOG(LogTemp, Warning, TEXT("[ATTACK DEBUG] Direct LeftMouseButton received."));
	Attack();
}

void APlayerCharacter::DebugDirectXAttack()
{
	UE_LOG(LogTemp, Warning, TEXT("[ATTACK DEBUG] Direct X received."));
	Attack();
}