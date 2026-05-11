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
	
	UFUNCTION(BlueprintCallable, Category = "Gameplay UI")
	void HandleMatchFinished(EGameplayResult Result);

	UFUNCTION(BlueprintCallable, Category = "Gameplay UI")
	UGameplayHUDWidget* GetGameplayHUDWidget() const;
	
	UFUNCTION(BlueprintCallable)
	void TogglePauseMenu();

	UFUNCTION(BlueprintCallable)
	void ResumeGame();

	UFUNCTION(BlueprintCallable)
	void ReturnToMainMenu();

	UFUNCTION(BlueprintCallable)
	void RestartCurrentLevel();

	UFUNCTION(BlueprintCallable)
	void ShowGameplayHUD();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UUserWidget> PauseMenuWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UUserWidget> GameplayHUDWidgetClass;

private:
	UPROPERTY()
	TObjectPtr<UPauseMenuWidget> PauseMenuWidget;

	UPROPERTY()
	TObjectPtr<UGameplayHUDWidget> GameplayHUDWidget;

	bool bIsPauseMenuOpen = false;

	void OpenPauseMenu();
	void ClosePauseMenu();
	void SetGameplayInputEnabled(bool bEnabled);
};