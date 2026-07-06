#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "TDShootAbility.generated.h"

UCLASS()
class MOTORES_II_API UTDShootAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UTDShootAbility();

protected:
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;
};