#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyCharacter.generated.h"

class UHealthComponent;
class APlayerCharacter;
class UAnimMontage;

UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	Patrol UMETA(DisplayName = "Patrol"),
	Chase UMETA(DisplayName = "Chase"),
	Attack UMETA(DisplayName = "Attack"),
	Dead UMETA(DisplayName = "Dead")
};

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

	UFUNCTION(BlueprintPure, Category = "Enemy|State")
	EEnemyState GetCurrentState() const;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UHealthComponent* HealthComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|State")
	EEnemyState CurrentState = EEnemyState::Patrol;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float AttackDamage = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float AttackRange = 180.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float DetectionRange = 750.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float LoseSightRange = 1100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float MaxChaseHeightDifference = 180.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float AttackCooldown = 1.25f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float PatrolSpeed = 180.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float ChaseSpeed = 350.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float PatrolAcceptanceRadius = 80.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float PatrolSwitchDistance = 120.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float ChaseAcceptanceRadius = 40.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	bool bLockDepthAxis = true;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Movement")
	TArray<AActor*> PatrolPoints;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	UAnimMontage* AttackMontage = nullptr;

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
	APlayerCharacter* TargetPlayer = nullptr;

	FTimerHandle AttackTimerHandle;

	int32 CurrentPatrolPointIndex = 0;

	float LockedDepthY = 0.0f;

	bool bCanAttack = true;
	bool bIsDead = false;

	void FindTargetPlayer();

	void UpdateEnemyBehavior();
	void SetEnemyState(EEnemyState NewState, bool bForce = false);

	void HandlePatrolState();
	void HandleChaseState();
	void HandleAttackState();
	void HandleDeadState();

	void MoveToCurrentPatrolPoint();
	void SelectNextPatrolPoint();
	void ChaseTarget();
	void MoveAlongX(float TargetX, float AcceptanceDistance);

	void FaceTarget();
	void StopMovement();

	void TryAttackTarget();
	void ResetAttack();

	float GetHorizontalDistanceToTarget() const;
	float GetHorizontalDistanceToLocation(const FVector& Location) const;
	float GetHeightDifferenceToTarget() const;

	bool IsTargetValidForDetection() const;
	bool IsTargetInsideDetectionRange() const;
	bool IsTargetInsideLoseSightRange() const;
	bool IsTargetInsideAttackRange() const;
	bool IsTargetInsideAllowedHeight() const;

	void LockDepthAxisPosition();
};