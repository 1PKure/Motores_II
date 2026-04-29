#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

#include "GameplayGameMode.generated.h"

class UGameplayHUDWidget;
class UEndGameWidget;

UCLASS()
class MOTORES_II_API AGameplayGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	void RestartGameplayLevel();
	void ReturnToMainMenu();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UGameplayHUDWidget> GameplayHUDWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UEndGameWidget> DefeatWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Levels")
	FName MainMenuLevelName = "MainMenu";

	UPROPERTY()
	UGameplayHUDWidget* GameplayHUDWidgetInstance;

	UPROPERTY()
	UEndGameWidget* DefeatWidgetInstance;

	UFUNCTION()
	void HandlePlayerDeath();

private:
	void ShowDefeatScreen();
};