#include "GameModes/GameplayGameMode.h"

#include "Controllers/GamePlayerController.h"
#include "Kismet/GameplayStatics.h"

AGameplayGameMode::AGameplayGameMode()
{
	MainMenuLevelName = TEXT("LV_MainMenu");
	CurrentResult = EGameplayResult::None;
	RequiredEnemiesToWin = 3;
	DefeatedEnemies = 0;
}

void AGameplayGameMode::BeginPlay()
{
	Super::BeginPlay();

	CurrentResult = EGameplayResult::None;
	DefeatedEnemies = 0;
}

void AGameplayGameMode::HandleVictory()
{
	FinishMatch(EGameplayResult::Victory);
}

void AGameplayGameMode::HandleDefeat()
{
	FinishMatch(EGameplayResult::Defeat);
}

void AGameplayGameMode::NotifyEnemyKilled()
{
	if (IsMatchFinished())
	{
		return;
	}

	DefeatedEnemies++;

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("Enemy killed. Progress: %d / %d"),
		DefeatedEnemies,
		RequiredEnemiesToWin
	);

	if (DefeatedEnemies >= RequiredEnemiesToWin)
	{
		HandleVictory();
	}
}

int32 AGameplayGameMode::GetDefeatedEnemies() const
{
	return DefeatedEnemies;
}

int32 AGameplayGameMode::GetRequiredEnemiesToWin() const
{
	return RequiredEnemiesToWin;
}

void AGameplayGameMode::FinishMatch(EGameplayResult Result)
{
	if (IsMatchFinished())
	{
		return;
	}

	CurrentResult = Result;

	AGamePlayerController* GamePlayerController =
		Cast<AGamePlayerController>(UGameplayStatics::GetPlayerController(this, 0));

	if (!IsValid(GamePlayerController))
	{
		UE_LOG(LogTemp, Error, TEXT("FinishMatch failed: GamePlayerController is invalid."));
		return;
	}

	GamePlayerController->HandleMatchFinished(Result);

	UE_LOG(LogTemp, Warning, TEXT("Match finished. Result: %d"), static_cast<int32>(Result));
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

bool AGameplayGameMode::IsMatchFinished() const
{
	return CurrentResult != EGameplayResult::None;
}