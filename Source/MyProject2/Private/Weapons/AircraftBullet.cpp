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

	BulletMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BulletMesh"));
	BulletMesh->SetWorldScale3D(FVector(5.f));

	Collision = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	RootComponent = Collision;
	Collision->SetCollisionProfileName("BlockAllDynamic");
	Collision->SetSimulatePhysics(true);
	Collision->SetNotifyRigidBodyCollision(true);
	Collision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	BulletMesh->SetupAttachment(RootComponent);
}

void AAircraftBullet::BeginPlay()
{
	Super::BeginPlay();
	Collision->OnComponentHit.AddDynamic(this, &AAircraftBullet::OnHit);
	damage = BulletStat->Damage;
	if (GetOwner()) Owner = Cast<ABaseAircraft>(GetOwner());
}

// Called every frame
void AAircraftBullet::Tick(float DeltaTime)
{
	if (currentTime >= LifeTime) Destroy();
	currentTime += DeltaTime;

	Super::Tick(DeltaTime);

}

void AAircraftBullet::FireInDirection(const FVector& ShootDirection) 
{
	FVector Impulse = ShootDirection * BulletSpeed;
	Collision->AddImpulse(Impulse, NAME_None, true);
}

void AAircraftBullet::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) {
	
	if (OtherActor && OtherActor != this) 
	{
		if (OtherActor && OtherComp->GetCollisionObjectType() == ECC_WorldStatic) 
		{
			DestroyBullet(OtherActor);
		}
	}
}

void AAircraftBullet::DestroyBullet(AActor* OtherActor)
{
	if (!OtherActor || OtherActor == this || OtherActor == GetOwner()) return;

	if (OtherActor->Implements<UTeamInterface>())
	{
		EFaction OtherFaction = Owner->Faction;
		OtherFaction = ITeamInterface::Execute_GetFaction(OtherActor);
		if (OtherFaction == Owner->Faction) return;
	}

	if (OtherActor->Implements<UDamageableInterface>())
	{
		IDamageableInterface::Execute_OnDamage(OtherActor, this, damage);
	}
}