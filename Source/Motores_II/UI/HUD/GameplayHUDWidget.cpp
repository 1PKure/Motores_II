#include "UI/HUD/GameplayHUDWidget.h"

#include "Components/ProgressBar.h"
#include "Components/HealthComponent.h"

void UGameplayHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();
	UE_LOG(LogTemp, Warning, TEXT("GameplayHUD NativeConstruct: %s"), *GetName());
}

void UGameplayHUDWidget::InitializeHealth(UHealthComponent* HealthComp)
{
	if (!HealthComp)
	{
		return;
	}

	CachedHealthComponent = HealthComp;
	UE_LOG(LogTemp, Error, TEXT("InitializeHealth called on: %s"), *GetName());
	HealthComp->OnHealthChanged.RemoveDynamic(this, &UGameplayHUDWidget::OnHealthChanged);
	HealthComp->OnHealthChanged.AddDynamic(this, &UGameplayHUDWidget::OnHealthChanged);

	OnHealthChanged(
		HealthComp->GetCurrentHealth(),
		HealthComp->GetMaxHealth()
	);
}

void UGameplayHUDWidget::OnHealthChanged(float CurrentHealth, float MaxHealth)
{
	if (!HealthBar)
	{
		return;
	}

	const float HealthPercent = MaxHealth > 0.0f ? CurrentHealth / MaxHealth : 0.0f;
	UE_LOG(LogTemp, Error, TEXT("HUD UPDATE -> %f"), HealthPercent);
	HealthBar->SetPercent(HealthPercent);
	HealthBar->SynchronizeProperties();
}