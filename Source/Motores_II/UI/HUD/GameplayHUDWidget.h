#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "GameplayHUDWidget.generated.h"

class UProgressBar;
class UHealthComponent;
class UWidget;

UCLASS()
class MOTORES_II_API UGameplayHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Health")
	void InitializeHealth(UHealthComponent* HealthComp);

	UFUNCTION(BlueprintCallable, Category = "Gameplay HUD")
	void ShowVictoryScreen();

	UFUNCTION(BlueprintCallable, Category = "Gameplay HUD")
	void ShowDefeatScreen();

	UFUNCTION(BlueprintCallable, Category = "Gameplay HUD")
	void ShowGameplayMessage(const FText& Message);

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthBar;

	UPROPERTY(meta = (BindWidgetOptional))
	UWidget* VictoryPanel;

	UPROPERTY(meta = (BindWidgetOptional))
	UWidget* DefeatPanel;

	UFUNCTION()
	void OnHealthChanged(float CurrentHealth, float MaxHealth);

private:
	UPROPERTY()
	UHealthComponent* CachedHealthComponent;
};