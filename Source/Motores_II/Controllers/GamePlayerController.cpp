#include "Controllers/GamePlayerController.h"

#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "UI/Menu/PauseMenuWidget.h"
#include "UI/HUD/GameplayHUDWidget.h"
#include "Characters/PlayerCharacter.h"
#include "Components/HealthComponent.h"

void AGamePlayerController::BeginPlay()
{
	Super::BeginPlay();

	bShowMouseCursor = false;
	SetInputMode(FInputModeGameOnly());

	ShowGameplayHUD();
	SetGameplayInputEnabled(true);
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

	if (!GameplayHUDWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create GameplayHUDWidget."));
		return;
	}

	GameplayHUDWidget->AddToViewport(0);

	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetPawn());

	if (!IsValid(PlayerCharacter))
	{
		UE_LOG(LogTemp, Error, TEXT("GamePlayerController: PlayerCharacter is invalid."));
		return;
	}

	UHealthComponent* HealthComponent = PlayerCharacter->GetHealthComponent();

	if (!IsValid(HealthComponent))
	{
		UE_LOG(LogTemp, Error, TEXT("GamePlayerController: HealthComponent is invalid."));
		return;
	}

	GameplayHUDWidget->InitializeHealth(HealthComponent);
}

UGameplayHUDWidget* AGamePlayerController::GetGameplayHUDWidget() const
{
	return GameplayHUDWidget;
}

void AGamePlayerController::HandleMatchFinished(EGameplayResult Result)
{
	UGameplayStatics::SetGamePaused(this, true);
	SetGameplayInputEnabled(false);

	if (!IsValid(GameplayHUDWidget))
	{
		UE_LOG(LogTemp, Error, TEXT("HandleMatchFinished failed: GameplayHUDWidget is invalid."));
		return;
	}

	switch (Result)
	{
	case EGameplayResult::Victory:
		GameplayHUDWidget->ShowVictoryScreen();
		break;

	case EGameplayResult::Defeat:
		GameplayHUDWidget->ShowDefeatScreen();
		break;

	default:
		break;
	}
}

void AGamePlayerController::SetGameplayInputEnabled(bool bEnabled)
{
	APawn* ControlledPawn = GetPawn();

	if (IsValid(ControlledPawn))
	{
		if (bEnabled)
		{
			ControlledPawn->EnableInput(this);
		}
		else
		{
			ControlledPawn->DisableInput(this);
		}
	}

	bShowMouseCursor = !bEnabled;

	if (bEnabled)
	{
		SetInputMode(FInputModeGameOnly());
	}
	else
	{
		FInputModeUIOnly InputMode;
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		SetInputMode(InputMode);
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
		PauseMenuWidget->AddToViewport(10);
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