#pragma once

#include "CoreMinimal.h"
#include "SSTCharacter.h"

#include "PlayerCharacter.generated.h"

class UHealthComponent;
class APlayerProjectile;
class UInputAction;
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
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	TSubclassOf<APlayerProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	FVector ProjectileSpawnOffset = FVector(90.0f, 0.0f, 40.0f);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* AttackAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float AttackCooldown = 0.35f;

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
	FTimerHandle AttackTimerHandle;

	float DefaultWalkSpeed;

	bool bCanAttack = true;

	void ResetSpeedBoost();

	void Attack();
	void ResetAttack();
	FVector GetAttackDirection() const;
	void AttackInputStarted();
	void AttackInputTriggered();
	void AttackInputCompleted();

	void DebugDirectMouseAttack();
	void DebugDirectXAttack();
};