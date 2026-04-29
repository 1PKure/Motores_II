#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EndGameWidget.generated.h"

class UButton;
class AGameplayGameMode;

UCLASS()
class MOTORES_II_API UEndGameWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void InitializeEndGameWidget(AGameplayGameMode* GameplayGameMode);

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UButton* RestartButton;

	UPROPERTY(meta = (BindWidget))
	UButton* MainMenuButton;

private:
	UPROPERTY()
	AGameplayGameMode* CachedGameplayGameMode;

	UFUNCTION()
	void HandleRestartClicked();

	UFUNCTION()
	void HandleMainMenuClicked();
};