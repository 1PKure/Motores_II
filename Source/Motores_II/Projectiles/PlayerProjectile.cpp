#include "PlayerProjectile.h"

#include "Motores_II/Characters/EnemyCharacter.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"


APlayerProjectile::APlayerProjectile()
{
	PrimaryActorTick.bCanEverTick = false;
	UE_LOG(LogTemp, Warning, TEXT("[PROJECTILE DEBUG] Constructor called."));
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	SetRootComponent(CollisionComponent);

	CollisionComponent->InitSphereRadius(18.0f);
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComponent->SetCollisionObjectType(ECC_WorldDynamic);
	CollisionComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	CollisionComponent->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap);
	CollisionComponent->SetGenerateOverlapEvents(true);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->InitialSpeed = 1200.0f;
	ProjectileMovementComponent->MaxSpeed = 1200.0f;
	ProjectileMovementComponent->ProjectileGravityScale = 0.0f;
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->bShouldBounce = false;
}

void APlayerProjectile::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("[PROJECTILE DEBUG] BeginPlay called. Projectile: %s"), *GetName());

	if (IsValid(CollisionComponent))
	{
		CollisionComponent->OnComponentBeginOverlap.AddDynamic(
			this,
			&APlayerProjectile::HandleBeginOverlap
		);
	}

	SetLifeSpan(LifeTime);
}

void APlayerProjectile::InitializeProjectile(const FVector& Direction)
{
	UE_LOG(LogTemp, Warning, TEXT("[PROJECTILE DEBUG] InitializeProjectile called. Direction: %s"), *Direction.ToString());

	if (!IsValid(ProjectileMovementComponent))
	{
		UE_LOG(LogTemp, Error, TEXT("InitializeProjectile failed: ProjectileMovementComponent is invalid."));
		return;
	}

	const FVector SafeDirection = Direction.GetSafeNormal();

	if (SafeDirection.IsNearlyZero())
	{
		UE_LOG(LogTemp, Warning, TEXT("InitializeProjectile failed: Direction is nearly zero."));
		return;
	}

	ProjectileMovementComponent->Velocity =
		SafeDirection * ProjectileMovementComponent->InitialSpeed;
	UE_LOG(
		LogTemp,
		Warning,
		TEXT("[PROJECTILE DEBUG] Velocity set to: %s"),
		*ProjectileMovementComponent->Velocity.ToString()
	);
}

void APlayerProjectile::HandleBeginOverlap(
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

	if (OtherActor == this || OtherActor == GetOwner())
	{
		return;
	}

	if (OtherActor->IsA<AEnemyCharacter>())
	{
		UGameplayStatics::ApplyDamage(
			OtherActor,
			Damage,
			GetInstigatorController(),
			this,
			nullptr
		);

		UE_LOG(LogTemp, Warning, TEXT("Projectile hit enemy. Damage: %f"), Damage);

		Destroy();
		return;
	}

	if (bDestroyOnAnyHit)
	{
		Destroy();
	}
}