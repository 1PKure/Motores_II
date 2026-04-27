#include "Characters/PlayerCharacter.h"
#include "Components/HealthComponent.h"
#include "Components/InputComponent.h"
#include "InputCoreTypes.h"

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
}