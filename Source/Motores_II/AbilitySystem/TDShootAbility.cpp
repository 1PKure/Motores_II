#include "AbilitySystem/TDShootAbility.h"

#include "Characters/PlayerCharacter.h"

UTDShootAbility::UTDShootAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UTDShootAbility::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	UE_LOG(LogTemp, Warning, TEXT("TDShootAbility::ActivateAbility called."));

	if (!ActorInfo || !ActorInfo->AvatarActor.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("GA_Shoot failed: ActorInfo or AvatarActor is invalid."));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(ActorInfo->AvatarActor.Get());

	if (!IsValid(PlayerCharacter))
	{
		UE_LOG(LogTemp, Error, TEXT("GA_Shoot failed: AvatarActor is not APlayerCharacter."));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		UE_LOG(LogTemp, Warning, TEXT("GA_Shoot failed: CommitAbility returned false."));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	const bool bProjectileSpawned = PlayerCharacter->SpawnProjectileFromAbility();

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("GA_Shoot projectile spawn result: %s"),
		bProjectileSpawned ? TEXT("true") : TEXT("false")
	);

	if (!bProjectileSpawned)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, false, false);
}