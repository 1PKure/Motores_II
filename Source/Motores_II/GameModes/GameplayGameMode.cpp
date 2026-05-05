#include "GameModes/GameplayGameMode.h"

#include "UI/HUD/GameplayHUDWidget.h"
#include "Public/Widgets/EndGameWidget.h"

#include "Characters/PlayerCharacter.h"
#include "Components/HealthComponent.h"

#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

void AGameplayGameMode::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	if (!PlayerController)
	{
		return;
	}

	// Limpia cualquier HUD previo antes de crear el oficial.
	RemoveDuplicatedGameplayHUDWidgets();

	if (GameplayHUDWidgetClass)
	{
		GameplayHUDWidgetInstance = CreateWidget<UGameplayHUDWidget>(
			PlayerController,
			GameplayHUDWidgetClass
		);

		if (GameplayHUDWidgetInstance)
		{
			GameplayHUDWidgetInstance->AddToViewport(9999);
		}
	}

	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(PlayerController->GetPawn());
	if (!PlayerCharacter)
	{
		return;
	}

	UHealthComponent* HealthComponent = PlayerCharacter->GetHealthComponent();
	if (!HealthComponent)
	{
		return;
	}

	if (GameplayHUDWidgetInstance)
	{
		GameplayHUDWidgetInstance->InitializeHealth(HealthComponent);
	}

	HealthComponent->OnDeath.RemoveDynamic(this, &AGameplayGameMode::HandlePlayerDeath);
	HealthComponent->OnDeath.AddDynamic(this, &AGameplayGameMode::HandlePlayerDeath);

	GetWorldTimerManager().SetTimerForNextTick(this, &AGameplayGameMode::RemoveDuplicatedGameplayHUDWidgets);
}

void AGameplayGameMode::HandlePlayerDeath()
{
	UE_LOG(LogTemp, Error, TEXT("GameMode received death"));
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

void AGameplayGameMode::RemoveDuplicatedGameplayHUDWidgets()
{
	if (!GameplayHUDWidgetClass)
	{
		return;
	}

	TArray<UUserWidget*> FoundWidgets;
	UWidgetBlueprintLibrary::GetAllWidgetsOfClass(
		this,
		FoundWidgets,
		GameplayHUDWidgetClass,
		false
	);

	for (UUserWidget* Widget : FoundWidgets)
	{
		if (!Widget)
		{
			continue;
		}
		
		if (Widget == GameplayHUDWidgetInstance)
		{
			continue;
		}

		UE_LOG(LogTemp, Warning, TEXT("Removing duplicated Gameplay HUD: %s"), *Widget->GetName());
		Widget->RemoveFromParent();
	}
}