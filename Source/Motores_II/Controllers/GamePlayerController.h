#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameModes/GameplayGameMode.h"
#include "GamePlayerController.generated.h"

class UUserWidget;
class UPauseMenuWidget;
class UGameplayHUDWidget;

UCLASS()
class MOTORES_II_API AGamePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void OnPossess(APawn* InPawn) override;

	UFUNCTION(BlueprintCallable, Category = "Gameplay UI")
	void HandleMatchFinished(EGameplayResult Result);

	UFUNCTION(BlueprintCallable, Category = "Gameplay UI")
	UGameplayHUDWidget* GetGameplayHUDWidget() const;

	UFUNCTION(BlueprintCallable, Category = "Gameplay UI")
	void ShowGameplayHUD();

	UFUNCTION(BlueprintCallable, Category = "Pause")
	void TogglePauseMenu();

	UFUNCTION(BlueprintCallable, Category = "Pause")
	void ResumeGame();

	UFUNCTION(BlueprintCallable, Category = "Level Flow")
	void ReturnToMainMenu();

	UFUNCTION(BlueprintCallable, Category = "Level Flow")
	void RestartCurrentLevel();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UUserWidget> PauseMenuWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UUserWidget> GameplayHUDWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Levels")
	FName MainMenuLevelName = TEXT("LV_MainMenu");

private:
	UPROPERTY()
	TObjectPtr<UPauseMenuWidget> PauseMenuWidget;

	UPROPERTY()
	TObjectPtr<UGameplayHUDWidget> GameplayHUDWidget;

	bool bIsPauseMenuOpen = false;
	bool bMatchFinished = false;

	void OpenPauseMenu();
	void ClosePauseMenu();
	void SetGameplayInputEnabled(bool bEnabled);
	void InitializeGameplayHUDFromPawn(APawn* PawnToInitialize);
};