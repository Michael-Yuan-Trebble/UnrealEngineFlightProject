// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Missile!"));
#include "Weapons/Missiles/BaseMissile.h"
#include "Aircraft/BaseAircraft.h"

ABaseMissile::ABaseMissile() 
{
	PrimaryActorTick.bCanEverTick = true;
	canLock = true;
	Collision = CreateDefaultSubobject<UBoxComponent>(TEXT("Missile Collision"));
	RootComponent = Collision;

	Collision->SetCollisionProfileName(TEXT("Projectile"));
	Collision->SetNotifyRigidBodyCollision(true);
	Collision->SetGenerateOverlapEvents(false);

	Collision->OnComponentBeginOverlap.AddDynamic(this, &ABaseMissile::OnOverlapBegin);
	Collision->OnComponentHit.AddDynamic(this, &ABaseMissile::OnHit);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->InitialSpeed = 0;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = 0.f;

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Missile"));
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponMesh->SetupAttachment(RootComponent);
}

void ABaseMissile::BeginPlay() 
{
	Super::BeginPlay();
	if (GetOwner()) 
	{
		Collision->IgnoreActorWhenMoving(GetOwner(), true);
		Owner = Cast<ABaseAircraft>(GetOwner());
	}
}

void ABaseMissile::Tick(float DeltaTime) 
{
	Super::Tick(DeltaTime);

}

void ABaseMissile::LaunchSequence(float speed) 
{

}

void ABaseMissile::activateSmoke() 
{
	if (!WeaponMesh || !WeaponMesh->DoesSocketExist(TEXT("ExhaustSocket"))) return;

	if (SmokeTrail != nullptr && !SmokeTrailSystem) return;
	if (MissileRocket != nullptr && !MissileRocketSystem) return;

	SmokeTrail = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		GetWorld(),
		SmokeTrailSystem,
		WeaponMesh->GetSocketLocation(TEXT("ExhaustSocket")),
		WeaponMesh->GetSocketRotation(TEXT("ExhaustSocket")),
		FVector(1.f),
		true,
		true
	);

	MissileRocket = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		GetWorld(),
		MissileRocketSystem,
		WeaponMesh->GetSocketLocation(TEXT("ExhaustSocket")),
		WeaponMesh->GetSocketRotation(TEXT("ExhaustSocket")),
		FVector(1.f),
		true,
		true
	);

	ProjectileMovement->Velocity = GetActorForwardVector() * ProjectileMovement->InitialSpeed;
}

void ABaseMissile::OnOverlapBegin(UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	CheckAndDelete(OtherActor);
}

void ABaseMissile::OnHit(UPrimitiveComponent* HitComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	FVector NormalImpulse,
	const FHitResult& Hit)
{
	CheckAndDelete(OtherActor);
}

void ABaseMissile::CheckAndDelete(AActor* OtherActor)
{

}

void ABaseMissile::DestroyMissile()
{
	if (bDestroyed) return;
	bDestroyed = true;

	if (ExplosionEffect) {
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			ExplosionEffect,
			GetActorLocation(),
			GetActorRotation()
		);
	}

	if (SmokeTrail) SmokeTrail->Deactivate();
	if (MissileRocket) MissileRocket->Deactivate();
	Destroy();
}