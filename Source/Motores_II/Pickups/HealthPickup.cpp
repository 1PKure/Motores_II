// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickups/HealthPickup.h"

#include "Public/Components/HealthComponent.h"


AHealthPickup::AHealthPickup()
{
	HealAmount = 25.0f;
}

void AHealthPickup::ApplyPickupEffect_Implementation(AActor* Collector)
{
	if (!IsValid(Collector))
	{
		return;
	}

	UHealthComponent* HealthComponent = Collector->FindComponentByClass<UHealthComponent>();

	if (!IsValid(HealthComponent))
	{
		UE_LOG(LogTemp, Warning, TEXT("HealthPickup failed: Collector has no HealthComponent."));
		return;
	}

	HealthComponent->Heal(HealAmount);

	UE_LOG(LogTemp, Warning, TEXT("HealthPickup applied. Heal Amount: %f"), HealAmount);
}