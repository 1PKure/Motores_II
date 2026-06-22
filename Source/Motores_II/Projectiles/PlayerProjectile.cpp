#include "Projectiles/PlayerProjectile.h"

#include "Characters/EnemyCharacter.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

APlayerProjectile::APlayerProjectile()
{
	PrimaryActorTick.bCanEverTick = false;

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	SetRootComponent(CollisionComponent);

	CollisionComponent->InitSphereRadius(18.0f);
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionComponent->SetCollisionObjectType(ECC_WorldDynamic);

	CollisionComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	CollisionComponent->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	CollisionComponent->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);

	CollisionComponent->SetGenerateOverlapEvents(true);
	CollisionComponent->SetNotifyRigidBodyCollision(true);

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

	if (IsValid(CollisionComponent))
	{
		CollisionComponent->OnComponentBeginOverlap.AddDynamic(
			this,
			&APlayerProjectile::HandleBeginOverlap
		);

		CollisionComponent->OnComponentHit.AddDynamic(
			this,
			&APlayerProjectile::HandleHit
		);
	}

	SetLifeSpan(LifeTime);
}

void APlayerProjectile::InitializeProjectile(const FVector& Direction)
{
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
	if (bHasImpacted || !IsValid(OtherActor))
	{
		return;
	}

	if (OtherActor == this || OtherActor == GetOwner())
	{
		return;
	}

	if (TryDamageEnemy(OtherActor))
	{
		FinishImpact();
		return;
	}

	if (bDestroyOnAnyHit)
	{
		FinishImpact();
	}
}

void APlayerProjectile::HandleHit(
	UPrimitiveComponent* HitComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComponent,
	FVector NormalImpulse,
	const FHitResult& Hit
)
{
	if (bHasImpacted || !IsValid(OtherActor))
	{
		return;
	}

	if (OtherActor == this || OtherActor == GetOwner())
	{
		return;
	}

	if (TryDamageEnemy(OtherActor))
	{
		FinishImpact();
		return;
	}

	if (bDestroyOnAnyHit)
	{
		FinishImpact();
	}
}

bool APlayerProjectile::TryDamageEnemy(AActor* OtherActor)
{
	AEnemyCharacter* EnemyCharacter = Cast<AEnemyCharacter>(OtherActor);

	if (!IsValid(EnemyCharacter))
	{
		return false;
	}

	UGameplayStatics::ApplyDamage(
		EnemyCharacter,
		Damage,
		GetInstigatorController(),
		this,
		nullptr
	);

	return true;
}

void APlayerProjectile::FinishImpact()
{
	if (bHasImpacted)
	{
		return;
	}

	bHasImpacted = true;
	Destroy();
}