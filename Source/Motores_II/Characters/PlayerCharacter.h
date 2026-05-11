#pragma once

#include "CoreMinimal.h"
#include "SSTCharacter.h"

#include "PlayerCharacter.generated.h"

class UHealthComponent;

UCLASS()
class MOTORES_II_API APlayerCharacter : public ASSTCharacter
{
	GENERATED_BODY()

public:
	APlayerCharacter(const FObjectInitializer& ObjectInitializer);
	
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintPure, Category = "Components")
	UHealthComponent* GetHealthComponent() const;
	
	UFUNCTION(BlueprintCallable, Category = "Powerups")
	void ApplySpeedBoost(float SpeedMultiplier, float Duration);

	void TestDamage();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UHealthComponent* HealthComponent;

	UFUNCTION()
	void HandleDeath();
	
	UFUNCTION()
	void HandleAnyDamage(
		AActor* DamagedActor,
		float Damage,
		const UDamageType* DamageType,
		AController* InstigatedBy,
		AActor* DamageCauser
	);
	
private:
	FTimerHandle SpeedBoostTimerHandle;

	float DefaultWalkSpeed;

	void ResetSpeedBoost();
};