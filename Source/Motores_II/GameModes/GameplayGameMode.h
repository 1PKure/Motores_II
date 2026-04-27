#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GameplayGameMode.generated.h"

class UGameplayHUDWidget;
class UUserWidget;

UCLASS()
class MOTORES_II_API AGameplayGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UGameplayHUDWidget> GameplayHUDWidgetClass;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UUserWidget> DefeatWidgetClass;

	UPROPERTY()
	UGameplayHUDWidget* GameplayHUDWidgetInstance;
	
	UPROPERTY()
	UUserWidget* DefeatWidgetInstance;

	UFUNCTION()
	void HandlePlayerDeath();

	void ShowDefeatScreen();
};