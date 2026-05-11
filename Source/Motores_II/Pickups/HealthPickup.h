#pragma once

#include "CoreMinimal.h"
#include "Pickups/PickupBase.h"
#include "HealthPickup.generated.h"

UCLASS()
class MOTORES_II_API AHealthPickup : public APickupBase
{
	GENERATED_BODY()

public:
	AHealthPickup();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Health Pickup")
	float HealAmount;

	virtual void ApplyPickupEffect_Implementation(AActor* Collector) override;
	
};
