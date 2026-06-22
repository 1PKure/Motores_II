#pragma once

#include "CoreMinimal.h"
#include "Pickups/PickupBase.h"
#include "JumpPowerupPickup.generated.h"

UCLASS()
class MOTORES_II_API AJumpPowerupPickup : public APickupBase
{
	GENERATED_BODY()

public:
	AJumpPowerupPickup();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Jump Powerup")
	float JumpMultiplier = 1.5f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Jump Powerup")
	float Duration = 5.0f;

	virtual void ApplyPickupEffect_Implementation(AActor* Collector) override;
};