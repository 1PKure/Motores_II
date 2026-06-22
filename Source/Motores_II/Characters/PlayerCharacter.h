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

	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintPure, Category = "Components")
	UHealthComponent* GetHealthComponent() const;

	UFUNCTION(BlueprintCallable, Category = "Powerups")
	void ApplySpeedBoost(float SpeedMultiplier, float Duration);

	UFUNCTION(BlueprintCallable, Category = "Powerups")
	void ApplyJumpBoost(float JumpMultiplier, float Duration);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UHealthComponent* HealthComponent = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	TSubclassOf<APlayerProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	FVector ProjectileSpawnOffset = FVector(90.0f, 0.0f, 40.0f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* AttackAction = nullptr;

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
	FTimerHandle JumpBoostTimerHandle;
	FTimerHandle AttackTimerHandle;

	float DefaultWalkSpeed = 0.0f;
	float DefaultJumpZVelocity = 0.0f;

	bool bCanAttack = true;

	void ResetSpeedBoost();
	void ResetJumpBoost();

	void Attack();
	void ResetAttack();

	FVector GetAttackDirection() const;

	void AttackInputStarted();
};