#include "PickupBase.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"

APickupBase::APickupBase()
{
	PrimaryActorTick.bCanEverTick = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	PickupMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PickupMesh"));
	PickupMesh->SetupAttachment(SceneRoot);
	PickupMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	PickupCollision = CreateDefaultSubobject<USphereComponent>(TEXT("PickupCollision"));
	PickupCollision->SetupAttachment(SceneRoot);
	PickupCollision->SetSphereRadius(80.0f);
	PickupCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	PickupCollision->SetCollisionObjectType(ECC_WorldDynamic);
	PickupCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	PickupCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	bDestroyAfterPickup = true;
	bRotate = true;
	RotationSpeed = 90.0f;
}

void APickupBase::BeginPlay()
{
	Super::BeginPlay();

	PickupCollision->OnComponentBeginOverlap.AddDynamic(this, &APickupBase::HandlePickupOverlap);
}

void APickupBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bRotate)
	{
		AddActorLocalRotation(FRotator(0.0f, RotationSpeed * DeltaTime, 0.0f));
	}
}

void APickupBase::HandlePickupOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComponent,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
)
{
	if (!IsValid(OtherActor))
	{
		return;
	}

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);

	if (OtherActor != PlayerPawn)
	{
		return;
	}

	if (!CanBePickedUp(OtherActor))
	{
		return;
	}

	ApplyPickupEffect(OtherActor);
	OnPickupCollected(OtherActor);

	if (bDestroyAfterPickup)
	{
		Destroy();
	}
}

bool APickupBase::CanBePickedUp_Implementation(AActor* Collector) const
{
	return IsValid(Collector);
}

void APickupBase::ApplyPickupEffect_Implementation(AActor* Collector)
{
	UE_LOG(LogTemp, Warning, TEXT("Pickup collected: %s"), *GetName());
}