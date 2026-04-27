#include "GameModes/GameplayGameMode.h"
#include "UI/HUD/GameplayHUDWidget.h"
#include "Characters/PlayerCharacter.h"
#include "Components/HealthComponent.h"

#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"

void AGameplayGameMode::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);

	if (!PlayerController)
	{
		return;
	}

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

	HealthComponent->OnDeath.AddDynamic(this, &AGameplayGameMode::HandlePlayerDeath);
}

void AGameplayGameMode::HandlePlayerDeath()
{
	ShowDefeatScreen();
}

void AGameplayGameMode::ShowDefeatScreen()
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);

	if (!PlayerController || !DefeatWidgetClass)
	{
		return;
	}

	DefeatWidgetInstance = CreateWidget<UUserWidget>(
		PlayerController,
		DefeatWidgetClass
	);

	if (!DefeatWidgetInstance)
	{
		return;
	}

	DefeatWidgetInstance->AddToViewport();

	PlayerController->SetPause(true);
	PlayerController->bShowMouseCursor = true;

	FInputModeUIOnly InputMode;
	InputMode.SetWidgetToFocus(DefeatWidgetInstance->TakeWidget());
	PlayerController->SetInputMode(InputMode);
}