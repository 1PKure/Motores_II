#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayHUDWidget.generated.h"

UCLASS()
class MOTORES_II_API UGameplayHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, Category = "HUD")
	void UpdateHealth(float CurrentHealth, float MaxHealth);

	UFUNCTION(BlueprintImplementableEvent, Category = "HUD")
	void UpdateScore(int32 NewScore);

	UFUNCTION(BlueprintImplementableEvent, Category = "HUD")
	void UpdateTimer(float RemainingTime);
};