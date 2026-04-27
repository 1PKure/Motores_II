#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayHUDWidget.generated.h"

class UProgressBar;
class UHealthComponent;

UCLASS()
class MOTORES_II_API UGameplayHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void InitializeHealth(UHealthComponent* HealthComp);

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthBar;

	UFUNCTION()
	void OnHealthChanged(float CurrentHealth, float MaxHealth);

private:
	UPROPERTY()
	UHealthComponent* CachedHealthComponent;
};