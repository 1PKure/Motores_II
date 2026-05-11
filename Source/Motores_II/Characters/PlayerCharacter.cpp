#include "Characters/PlayerCharacter.h"
#include "Components/HealthComponent.h"
#include "Components/InputComponent.h"
#include "InputCoreTypes.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameModes/GameplayGameMode.h"
#include "Kismet/GameplayStatics.h"

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

	if (PlayerInputComponent)
	{
		PlayerInputComponent->BindKey(EKeys::H, IE_Pressed, this, &APlayerCharacter::TestDamage);
	}
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