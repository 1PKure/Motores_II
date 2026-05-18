#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "EnemyCharacter.generated.h"

class UHealthComponent;
class APlayerCharacter;

UCLASS()
class MOTORES_II_API AEnemyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AEnemyCharacter();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintPure, Category = "Components")
	UHealthComponent* GetHealthComponent() const;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UHealthComponent* HealthComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float AttackDamage = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float AttackRange = 150.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float DetectionRange = 900.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float AttackCooldown = 1.25f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float ChaseSpeed = 350.0f;

	UFUNCTION()
	void HandleAnyDamage(
		AActor* DamagedActor,
		float Damage,
		const UDamageType* DamageType,
		AController* InstigatedBy,
		AActor* DamageCauser
	);

	UFUNCTION()
	void HandleDeath();

private:
	UPROPERTY()
	APlayerCharacter* TargetPlayer;

	FTimerHandle AttackTimerHandle;

	bool bCanAttack;
	bool bIsDead;

	void FindTargetPlayer();
	void UpdateEnemyBehavior();
	void ChaseTarget();
	void TryAttackTarget();
	void ResetAttack();
	float GetDistanceToTarget() const;
};