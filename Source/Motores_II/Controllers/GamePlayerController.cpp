#include "Controllers/GamePlayerController.h"

#include "Blueprint/UserWidget.h"
#include "Characters/PlayerCharacter.h"
#include "Components/HealthComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UI/HUD/GameplayHUDWidget.h"
#include "UI/Menu/PauseMenuWidget.h"

void AGamePlayerController::BeginPlay()
{
	Super::BeginPlay();

	bShowMouseCursor = false;
	SetInputMode(FInputModeGameOnly());

	ShowGameplayHUD();
	InitializeGameplayHUDFromPawn(GetPawn());

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

void AGamePlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	InitializeGameplayHUDFromPawn(InPawn);
}

void AGamePlayerController::ShowGameplayHUD()
{
	if (!GameplayHUDWidgetClass)
	{
		UE_LOG(LogTemp, Error, TEXT("ShowGameplayHUD failed: GameplayHUDWidgetClass is not assigned."));
		return;
	}

	if (IsValid(GameplayHUDWidget))
	{
		return;
	}

	GameplayHUDWidget = CreateWidget<UGameplayHUDWidget>(this, GameplayHUDWidgetClass);

	if (!IsValid(GameplayHUDWidget))
	{
		UE_LOG(LogTemp, Error, TEXT("ShowGameplayHUD failed: widget creation returned null."));
		return;
	}

	GameplayHUDWidget->AddToViewport(0);
}

void AGamePlayerController::InitializeGameplayHUDFromPawn(APawn* PawnToInitialize)
{
	if (!IsValid(GameplayHUDWidget))
	{
		return;
	}

	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(PawnToInitialize);

	if (!IsValid(PlayerCharacter))
	{
		return;
	}

	UHealthComponent* HealthComponent = PlayerCharacter->GetHealthComponent();

	if (!IsValid(HealthComponent))
	{
		UE_LOG(LogTemp, Error, TEXT("InitializeGameplayHUDFromPawn failed: HealthComponent is invalid."));
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
	if (bMatchFinished || Result == EGameplayResult::None)
	{
		return;
	}

	bMatchFinished = true;

	if (IsValid(PauseMenuWidget) && PauseMenuWidget->IsInViewport())
	{
		PauseMenuWidget->RemoveFromParent();
	}

	bIsPauseMenuOpen = false;

	ShowGameplayHUD();

	SetPause(true);
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
	if (bMatchFinished)
	{
		return;
	}

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
	if (bIsPauseMenuOpen || bMatchFinished)
	{
		return;
	}

	SetPause(true);

	if (PauseMenuWidgetClass && !IsValid(PauseMenuWidget))
	{
		PauseMenuWidget = CreateWidget<UPauseMenuWidget>(this, PauseMenuWidgetClass);
	}

	if (IsValid(PauseMenuWidget) && !PauseMenuWidget->IsInViewport())
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

	if (IsValid(PauseMenuWidget) && PauseMenuWidget->IsInViewport())
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
	UGameplayStatics::OpenLevel(this, MainMenuLevelName);
}

void AGamePlayerController::RestartCurrentLevel()
{
	SetPause(false);

	const FName CurrentLevelName = FName(*UGameplayStatics::GetCurrentLevelName(this));
	UGameplayStatics::OpenLevel(this, CurrentLevelName);
}