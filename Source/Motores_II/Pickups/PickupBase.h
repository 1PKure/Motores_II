#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickupBase.generated.h"

class USphereComponent;
class UStaticMeshComponent;

UCLASS()
class MOTORES_II_API APickupBase : public AActor
{
	GENERATED_BODY()

public:
	APickupBase();

	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* PickupMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* PickupCollision;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup")
	bool bDestroyAfterPickup;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup")
	bool bRotate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup", meta = (EditCondition = "bRotate"))
	float RotationSpeed;

	UFUNCTION()
	void HandlePickupOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComponent,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UFUNCTION(BlueprintNativeEvent, Category = "Pickup")
	bool CanBePickedUp(AActor* Collector) const;

	virtual bool CanBePickedUp_Implementation(AActor* Collector) const;

	UFUNCTION(BlueprintNativeEvent, Category = "Pickup")
	void ApplyPickupEffect(AActor* Collector);

	virtual void ApplyPickupEffect_Implementation(AActor* Collector);

	UFUNCTION(BlueprintImplementableEvent, Category = "Pickup")
	void OnPickupCollected(AActor* Collector);
};