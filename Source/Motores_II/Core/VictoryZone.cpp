
#include "Core/VictoryZone.h"
#include "Components/BoxComponent.h"
#include "Motores_II/GameModes/GameplayGameMode.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
AVictoryZone::AVictoryZone()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->SetupAttachment(SceneRoot);
	TriggerBox->SetBoxExtent(FVector(150.0f, 150.0f, 150.0f));
	TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerBox->SetCollisionObjectType(ECC_WorldDynamic);
	TriggerBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	TriggerBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void AVictoryZone::BeginPlay()
{
	Super::BeginPlay();

	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AVictoryZone::HandleVictoryZoneOverlap);
}

void AVictoryZone::HandleVictoryZoneOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComponent,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
)
{
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);

	if (OtherActor != PlayerPawn)
	{
		return;
	}

	AGameplayGameMode* GameplayGameMode = Cast<AGameplayGameMode>(UGameplayStatics::GetGameMode(this));

	if (!IsValid(GameplayGameMode))
	{
		return;
	}

	GameplayGameMode->HandleVictory();
}