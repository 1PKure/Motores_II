#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

#include "GameplayGameMode.generated.h"

UENUM(BlueprintType)
enum class EGameplayResult : uint8
{
	None,
	Victory,
	Defeat
};

UCLASS()
class MOTORES_II_API AGameplayGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AGameplayGameMode();

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Gameplay Flow")
	void HandleVictory();

	UFUNCTION(BlueprintCallable, Category = "Gameplay Flow")
	void HandleDefeat();

	UFUNCTION(BlueprintCallable, Category = "Gameplay Flow")
	void RestartGameplayLevel();

	UFUNCTION(BlueprintCallable, Category = "Gameplay Flow")
	void ReturnToMainMenu();

	UFUNCTION(BlueprintCallable, Category = "Gameplay Flow")
	void NotifyEnemyKilled();

	UFUNCTION(BlueprintPure, Category = "Gameplay Flow")
	bool IsMatchFinished() const;

	UFUNCTION(BlueprintPure, Category = "Gameplay Objectives")
	int32 GetDefeatedEnemies() const;

	UFUNCTION(BlueprintPure, Category = "Gameplay Objectives")
	int32 GetRequiredEnemiesToWin() const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Levels")
	FName MainMenuLevelName = TEXT("LV_MainMenu");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay Objectives")
	int32 RequiredEnemiesToWin = 3;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gameplay Objectives")
	int32 DefeatedEnemies = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gameplay Flow")
	EGameplayResult CurrentResult;

private:
	void FinishMatch(EGameplayResult Result);
};