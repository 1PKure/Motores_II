


#include "Pickups/SpeedPowerupPickup.h"
#include "Characters/PlayerCharacter.h"

ASpeedPowerupPickup::ASpeedPowerupPickup()
{
	SpeedMultiplier = 1.5f;
	Duration = 5.0f;
}

void ASpeedPowerupPickup::ApplyPickupEffect_Implementation(AActor* Collector)
{
	if (!IsValid(Collector))
	{
		return;
	}
	
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(Collector);

	if (!IsValid(PlayerCharacter))
	{
		UE_LOG(LogTemp, Warning, TEXT("SpeedPowerup failed: Collector is not the expected player character."));
		return;
	}

	PlayerCharacter->ApplySpeedBoost(SpeedMultiplier, Duration);
}