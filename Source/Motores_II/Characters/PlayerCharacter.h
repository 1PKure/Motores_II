#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "SSTCharacter.h"
#include "PlayerCharacter.generated.h"

class UAbilitySystemComponent;
class UGameplayAbility;
class UHealthComponent;
class APlayerProjectile;
class UInputAction;

UCLASS()
class MOTORES_II_API APlayerCharacter : public ASSTCharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	APlayerCharacter(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void RequestShoot();
	
	UFUNCTION(BlueprintPure, Category = "Components")
	UHealthComponent* GetHealthComponent() const;

	UFUNCTION(BlueprintCallable, Category = "Powerups")
	void ApplySpeedBoost(float SpeedMultiplier, float Duration);

	UFUNCTION(BlueprintCallable, Category = "Powerups")
	void ApplyJumpBoost(float JumpMultiplier, float Duration);

	bool SpawnProjectileFromAbility();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UHealthComponent* HealthComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	UAbilitySystemComponent* AbilitySystemComponent = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS")
	TSubclassOf<UGameplayAbility> ShootAbilityClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	TSubclassOf<APlayerProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	FVector ProjectileSpawnOffset = FVector(90.0f, 0.0f, 40.0f);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> AttackAction;

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

	float DefaultWalkSpeed = 0.0f;
	float DefaultJumpZVelocity = 0.0f;

	void InitializeAbilitySystem();
	void GiveStartupAbilities();

	void ResetSpeedBoost();
	void ResetJumpBoost();

	void Attack();

	FVector GetAttackDirection() const;

	void AttackInputStarted();
};