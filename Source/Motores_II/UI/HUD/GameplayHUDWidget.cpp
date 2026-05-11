#include "UI/HUD/GameplayHUDWidget.h"

#include "Components/HealthComponent.h"
#include "Components/ProgressBar.h"
#include "Components/Widget.h"

void UGameplayHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UE_LOG(LogTemp, Warning, TEXT("[HUD FLOW] GameplayHUD NativeConstruct: %s"), *GetName());

	if (IsValid(VictoryPanel))
	{
		VictoryPanel->SetVisibility(ESlateVisibility::Collapsed);
		UE_LOG(LogTemp, Warning, TEXT("[HUD FLOW] VictoryPanel bound correctly."));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[HUD FLOW] VictoryPanel is NOT bound."));
	}

	if (IsValid(DefeatPanel))
	{
		DefeatPanel->SetVisibility(ESlateVisibility::Collapsed);
		UE_LOG(LogTemp, Warning, TEXT("[HUD FLOW] DefeatPanel bound correctly."));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[HUD FLOW] DefeatPanel is NOT bound."));
	}
}

void UGameplayHUDWidget::InitializeHealth(UHealthComponent* HealthComp)
{
	if (!IsValid(HealthComp))
	{
		UE_LOG(LogTemp, Error, TEXT("InitializeHealth failed: HealthComponent is invalid."));
		return;
	}

	CachedHealthComponent = HealthComp;

	HealthComp->OnHealthChanged.RemoveDynamic(this, &UGameplayHUDWidget::OnHealthChanged);
	HealthComp->OnHealthChanged.AddDynamic(this, &UGameplayHUDWidget::OnHealthChanged);

	OnHealthChanged(
		HealthComp->GetCurrentHealth(),
		HealthComp->GetMaxHealth()
	);
}

void UGameplayHUDWidget::OnHealthChanged(float CurrentHealth, float MaxHealth)
{
	if (!IsValid(HealthBar))
	{
		UE_LOG(LogTemp, Error, TEXT("OnHealthChanged failed: HealthBar is invalid."));
		return;
	}

	const float HealthPercent = MaxHealth > 0.0f ? CurrentHealth / MaxHealth : 0.0f;

	UE_LOG(LogTemp, Warning, TEXT("HUD Health Update: %f"), HealthPercent);

	HealthBar->SetPercent(HealthPercent);
}

void UGameplayHUDWidget::ShowVictoryScreen()
{
	UE_LOG(LogTemp, Warning, TEXT("[HUD FLOW] ShowVictoryScreen called on: %s"), *GetName());

	if (IsValid(DefeatPanel))
	{
		DefeatPanel->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (!IsValid(VictoryPanel))
	{
		UE_LOG(LogTemp, Error, TEXT("[HUD FLOW] VictoryPanel is invalid."));
		return;
	}

	VictoryPanel->SetVisibility(ESlateVisibility::Visible);
}

void UGameplayHUDWidget::ShowDefeatScreen()
{
	UE_LOG(LogTemp, Warning, TEXT("[HUD FLOW] ShowDefeatScreen called on: %s"), *GetName());

	if (IsValid(VictoryPanel))
	{
		VictoryPanel->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (!IsValid(DefeatPanel))
	{
		UE_LOG(LogTemp, Error, TEXT("[HUD FLOW] DefeatPanel is invalid."));
		return;
	}

	DefeatPanel->SetVisibility(ESlateVisibility::Visible);
}

void UGameplayHUDWidget::ShowGameplayMessage(const FText& Message)
{
	UE_LOG(LogTemp, Warning, TEXT("[HUD FLOW] Gameplay Message: %s"), *Message.ToString());
}