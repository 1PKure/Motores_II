#pragma once

#include "CoreMinimal.h"
#include "Pickups/PickupBase.h"
#include "SpeedPowerupPickup.generated.h"

UCLASS()
class MOTORES_II_API ASpeedPowerupPickup : public APickupBase
{
	GENERATED_BODY()
public:
	ASpeedPowerupPickup();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Speed Powerup")
	float SpeedMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Speed Powerup")
	float Duration;

	virtual void ApplyPickupEffect_Implementation(AActor* Collector) override;
};
