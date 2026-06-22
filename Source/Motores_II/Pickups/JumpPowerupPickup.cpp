#include "Pickups/JumpPowerupPickup.h"

#include "Characters/PlayerCharacter.h"

AJumpPowerupPickup::AJumpPowerupPickup()
{
	JumpMultiplier = 1.5f;
	Duration = 5.0f;
}

void AJumpPowerupPickup::ApplyPickupEffect_Implementation(AActor* Collector)
{
	if (!IsValid(Collector))
	{
		return;
	}

	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(Collector);

	if (!IsValid(PlayerCharacter))
	{
		UE_LOG(LogTemp, Warning, TEXT("JumpPowerup failed: Collector is not the expected player character."));
		return;
	}

	PlayerCharacter->ApplyJumpBoost(JumpMultiplier, Duration);
}