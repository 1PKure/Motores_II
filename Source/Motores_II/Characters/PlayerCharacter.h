#pragma once

#include "CoreMinimal.h"
#include "SSTCharacter.h"

#include "PlayerCharacter.generated.h"

class UHealthComponent;

UCLASS()
class MOTORES_II_API APlayerCharacter : public ASSTCharacter
{
	GENERATED_BODY()

public:
	APlayerCharacter(const FObjectInitializer& ObjectInitializer);
	
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintPure, Category = "Components")
	UHealthComponent* GetHealthComponent() const;
	
	void TestDamage();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UHealthComponent* HealthComponent;

	UFUNCTION()
	void HandleDeath();

};