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

	UFUNCTION(BlueprintPure, Category = "Gameplay Flow")
	bool IsMatchFinished() const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Levels")
	FName MainMenuLevelName = TEXT("LV_MainMenu");

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gameplay Flow")
	EGameplayResult CurrentResult;

private:
	void FinishMatch(EGameplayResult Result);
};