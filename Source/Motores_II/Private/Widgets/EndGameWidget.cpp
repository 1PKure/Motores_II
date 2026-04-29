#include "Public/Widgets/EndGameWidget.h"
#include "GameModes/GameplayGameMode.h"
#include "Components/Button.h"

void UEndGameWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (RestartButton)
	{
		RestartButton->OnClicked.AddDynamic(this, &UEndGameWidget::HandleRestartClicked);
	}

	if (MainMenuButton)
	{
		MainMenuButton->OnClicked.AddDynamic(this, &UEndGameWidget::HandleMainMenuClicked);
	}
}

void UEndGameWidget::InitializeEndGameWidget(AGameplayGameMode* GameplayGameMode)
{
	CachedGameplayGameMode = GameplayGameMode;
}

void UEndGameWidget::HandleRestartClicked()
{
	if (CachedGameplayGameMode)
	{
		CachedGameplayGameMode->RestartGameplayLevel();
	}
}

void UEndGameWidget::HandleMainMenuClicked()
{
	if (CachedGameplayGameMode)
	{
		CachedGameplayGameMode->ReturnToMainMenu();
	}
}