#include "Controllers/GamePlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "UI/Menu/PauseMenuWidget.h"
#include "UI/HUD/GameplayHUDWidget.h"

void AGamePlayerController::BeginPlay()
{
	Super::BeginPlay();

	bShowMouseCursor = false;
	SetInputMode(FInputModeGameOnly());

	ShowGameplayHUD();
}

void AGamePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (InputComponent)
	{
		InputComponent->BindAction("Pause", IE_Pressed, this, &AGamePlayerController::TogglePauseMenu);
	}
}

void AGamePlayerController::ShowGameplayHUD()
{
	if (!GameplayHUDWidgetClass || GameplayHUDWidget)
	{
		return;
	}

	GameplayHUDWidget = CreateWidget<UGameplayHUDWidget>(this, GameplayHUDWidgetClass);
	if (GameplayHUDWidget)
	{
		GameplayHUDWidget->AddToViewport();
	}
}

void AGamePlayerController::TogglePauseMenu()
{
	if (bIsPauseMenuOpen)
	{
		ClosePauseMenu();
	}
	else
	{
		OpenPauseMenu();
	}
}

void AGamePlayerController::OpenPauseMenu()
{
	if (bIsPauseMenuOpen)
	{
		return;
	}

	SetPause(true);

	if (PauseMenuWidgetClass && !PauseMenuWidget)
	{
		PauseMenuWidget = CreateWidget<UPauseMenuWidget>(this, PauseMenuWidgetClass);
	}

	if (PauseMenuWidget && !PauseMenuWidget->IsInViewport())
	{
		PauseMenuWidget->AddToViewport();
	}

	FInputModeUIOnly InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	SetInputMode(InputMode);

	bShowMouseCursor = true;
	bIsPauseMenuOpen = true;
}

void AGamePlayerController::ClosePauseMenu()
{
	if (!bIsPauseMenuOpen)
	{
		return;
	}

	if (PauseMenuWidget && PauseMenuWidget->IsInViewport())
	{
		PauseMenuWidget->RemoveFromParent();
	}

	SetPause(false);
	SetInputMode(FInputModeGameOnly());
	bShowMouseCursor = false;
	bIsPauseMenuOpen = false;
}

void AGamePlayerController::ResumeGame()
{
	ClosePauseMenu();
}

void AGamePlayerController::ReturnToMainMenu()
{
	SetPause(false);
	UGameplayStatics::OpenLevel(this, FName("LV_MainMenu"));
}

void AGamePlayerController::RestartCurrentLevel()
{
	SetPause(false);

	const FName CurrentLevelName = FName(*UGameplayStatics::GetCurrentLevelName(this));
	UGameplayStatics::OpenLevel(this, CurrentLevelName);
}