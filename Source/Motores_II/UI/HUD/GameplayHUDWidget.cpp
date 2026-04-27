#include "GameplayHUDWidget.h"
#include "Components/ProgressBar.h"
#include "Components/HealthComponent.h"

void UGameplayHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UGameplayHUDWidget::InitializeHealth(UHealthComponent* HealthComp)
{
	if (!HealthComp) return;

	CachedHealthComponent = HealthComp;
	
	HealthComp->OnHealthChanged.AddDynamic(this, &UGameplayHUDWidget::OnHealthChanged);
	
	OnHealthChanged(HealthComp->GetCurrentHealth(), HealthComp->GetMaxHealth());
}

void UGameplayHUDWidget::OnHealthChanged(float CurrentHealth, float MaxHealth)
{
	if (!HealthBar) return;

	const float Percent = (MaxHealth > 0.0f) ? CurrentHealth / MaxHealth : 0.0f;
	HealthBar->SetPercent(Percent);
}