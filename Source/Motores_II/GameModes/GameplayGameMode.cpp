#include "GameModes/GameplayGameMode.h"

#include "UI/HUD/GameplayHUDWidget.h"
#include "Public/Widgets/EndGameWidget.h"

#include "Characters/PlayerCharacter.h"
#include "Components/HealthComponent.h"

#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"

void AGameplayGameMode::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	if (!PlayerController) return;
	
	if (GameplayHUDWidgetClass)
	{
		GameplayHUDWidgetInstance = CreateWidget<UGameplayHUDWidget>(
			PlayerController,
			GameplayHUDWidgetClass
		);

		if (GameplayHUDWidgetInstance)
		{
			GameplayHUDWidgetInstance->AddToViewport();
		}
	}
	
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(PlayerController->GetPawn());
	if (!PlayerCharacter) return;

	UHealthComponent* HealthComponent = PlayerCharacter->GetHealthComponent();
	if (!HealthComponent) return;
	
	if (GameplayHUDWidgetInstance)
	{
		GameplayHUDWidgetInstance->InitializeHealth(HealthComponent);
	}
	
	HealthComponent->OnDeath.AddDynamic(this, &AGameplayGameMode::HandlePlayerDeath);
}

void AGameplayGameMode::HandlePlayerDeath()
{
	ShowDefeatScreen();
}

void AGameplayGameMode::ShowDefeatScreen()
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	if (!PlayerController || !DefeatWidgetClass) return;

	DefeatWidgetInstance = CreateWidget<UEndGameWidget>(
		PlayerController,
		DefeatWidgetClass
	);

	if (!DefeatWidgetInstance) return;

	DefeatWidgetInstance->InitializeEndGameWidget(this);
	DefeatWidgetInstance->AddToViewport(10000);

	PlayerController->SetPause(true);
	PlayerController->bShowMouseCursor = true;

	FInputModeUIOnly InputMode;
	InputMode.SetWidgetToFocus(DefeatWidgetInstance->TakeWidget());
	PlayerController->SetInputMode(InputMode);
}

void AGameplayGameMode::RestartGameplayLevel()
{
	UGameplayStatics::SetGamePaused(this, false);

	const FName LevelName = FName(*UGameplayStatics::GetCurrentLevelName(this));
	UGameplayStatics::OpenLevel(this, LevelName);
}

void AGameplayGameMode::ReturnToMainMenu()
{
	UGameplayStatics::SetGamePaused(this, false);
	UGameplayStatics::OpenLevel(this, MainMenuLevelName);
}