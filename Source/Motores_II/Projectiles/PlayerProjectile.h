#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "PlayerProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;

UCLASS()
class MOTORES_II_API APlayerProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	APlayerProjectile();

	void InitializeProjectile(const FVector& Direction);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* CollisionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UProjectileMovementComponent* ProjectileMovementComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float Damage = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float LifeTime = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bDestroyOnAnyHit = true;

	UFUNCTION()
	void HandleBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComponent,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);
};