// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Bullet!"));
#include "Weapons/AircraftBullet.h"
#include "Components/BoxComponent.h"
#include "Aircraft/BaseAircraft.h"
#include "Structs and Data/Weapon Data/BulletStats.h"
#include "Structs and Data/TeamInterface.h"
#include "Structs and Data/DamageableInterface.h"
#include "DrawDebugHelpers.h"

AAircraftBullet::AAircraftBullet()
{
	PrimaryActorTick.bCanEverTick = true;
	
	Collision = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	RootComponent = Collision;

	Collision->SetCollisionProfileName("Projectile");
	Collision->SetNotifyRigidBodyCollision(true);
	Collision->SetGenerateOverlapEvents(false);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = 1.f;
	ProjectileMovement->bAutoActivate = true;
	ProjectileMovement->bSweepCollision = true;
	ProjectileMovement->bForceSubStepping = true;

	BulletMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BulletMesh"));
	BulletMesh->SetWorldScale3D(FVector(5.f));
	BulletMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BulletMesh->SetupAttachment(RootComponent);
	
}

void AAircraftBullet::BeginPlay()
{
	Super::BeginPlay();
	damage = BulletStat->Damage;
	if (IsValid(GetOwner())) 
	{
		Owner = Cast<ABaseAircraft>(GetOwner());
		Collision->IgnoreActorWhenMoving(GetOwner(), true);
	}
	SetLifeSpan(LifeTime);
	Collision->OnComponentBeginOverlap.AddDynamic(this, &AAircraftBullet::OnOverlapBegin);
	Collision->OnComponentHit.AddDynamic(this, &AAircraftBullet::OnHit);
}

void AAircraftBullet::FireInDirection(const FVector& ShootDirection) 
{
	if (!ProjectileMovement) return;
	ProjectileMovement->SetUpdatedComponent(Collision);
	ProjectileMovement->Velocity = ShootDirection.GetSafeNormal() * BulletSpeed;
}

void AAircraftBullet::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) {
	
	DestroyBullet(OtherActor);
}

void AAircraftBullet::OnOverlapBegin(UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	DestroyBullet(OtherActor);
}

void AAircraftBullet::DestroyBullet(AActor* OtherActor)
{
	if (bDestroyed) return;
	bDestroyed = true;
	if (!OtherActor || OtherActor == this || OtherActor == GetOwner()) return;
	if (!Owner || !Owner->IsValidLowLevelFast()) return;

	if (OtherActor->Implements<UTeamInterface>())
	{
		EFaction OtherFaction = Owner->Faction;
		OtherFaction = ITeamInterface::Execute_GetFaction(OtherActor);
		if (OtherFaction == Owner->Faction) return;
	}

	if (OtherActor->Implements<UDamageableInterface>())
	{
		IDamageableInterface::Execute_OnDamage(OtherActor, this, Owner, OtherActor, damage);
	}

	if (ExplosionEffect) {
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			ExplosionEffect,
			GetActorLocation(),
			GetActorRotation()
		);
	}

	Destroy();
}