// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Missile!"));
#include "Weapons/Missiles/BaseIRMissile.h"
#include "NiagaraFunctionLibrary.h"
#include "Structs and Data/TeamInterface.h"
#include "Structs and Data/DamageableInterface.h"
#include "Aircraft/BaseAircraft.h"

ABaseIRMissile::ABaseIRMissile()
{
	PrimaryActorTick.bCanEverTick = true;
	canLock = true;
	Collision = CreateDefaultSubobject<UBoxComponent>(TEXT("Missile Collision"));
	RootComponent = Collision;

	Collision->SetCollisionProfileName(TEXT("Projectile"));
	Collision->SetNotifyRigidBodyCollision(true);
	Collision->SetGenerateOverlapEvents(false);

	Collision->OnComponentBeginOverlap.AddDynamic(this, &ABaseIRMissile::OnOverlapBegin);
	Collision->OnComponentHit.AddDynamic(this, &ABaseIRMissile::OnHit);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->InitialSpeed = 0;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = 0.f;

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Missile"));
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponMesh->SetupAttachment(RootComponent);
}

void ABaseIRMissile::BeginPlay()
{
	Super::BeginPlay();
	if (GetOwner())
	{
		Collision->IgnoreActorWhenMoving(GetOwner(), true);
		Owner = Cast<ABaseAircraft>(GetOwner());
	}
	if (!MissileStats) return;
	WeaponName = MissileStats->InGameName;
	missileAcceleration = MissileStats->Acceleration;
	missileMaxSpeed = MissileStats->MaxSpeed;
	cooldownTime = MissileStats->Cooldown;
	range = MissileStats->LockOnRange;
	turnRate = MissileStats->TurnRate;
	ProjectileMovement->MaxSpeed = MissileStats->MaxSpeed;
}

void ABaseIRMissile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!isAir) return;

	if (!SmokeTrail || !MissileRocket) activateSmoke();

	ProjectileMovement->Velocity += GetActorForwardVector() * missileAcceleration * DeltaTime;
	ProjectileMovement->Velocity = ProjectileMovement->Velocity.GetClampedToMaxSize(ProjectileMovement->MaxSpeed);

	if (SmokeTrail) 
	{
		SmokeTrail->SetWorldLocation(WeaponMesh->GetSocketLocation(TEXT("ExhaustSocket")));
		SmokeTrail->SetWorldRotation(WeaponMesh->GetSocketRotation(TEXT("ExhaustSocket")));
	}

	if (MissileRocket)
	{
		MissileRocket->SetWorldLocation(WeaponMesh->GetSocketLocation(TEXT("ExhaustSocket")));
		MissileRocket->SetWorldRotation(WeaponMesh->GetSocketRotation(TEXT("ExhaustSocket")));
	}

	timeTilDelt += DeltaTime;

	if (!(timeTilDelt >= MissileStats->LifeTime)) return;

	DestroyMissile();
}

void ABaseIRMissile::FireStatic(float launchSpeed)
{
	LaunchSequence(launchSpeed);
}

void ABaseIRMissile::FireTracking(float launchSpeed, AActor* Target) 
{
	Tracking = Target;
	if (Tracking)
	{
		ProjectileMovement->bIsHomingProjectile = true;
		ProjectileMovement->HomingTargetComponent = Tracking->GetRootComponent();
		ProjectileMovement->HomingAccelerationMagnitude = turnRate;
	}
	LaunchSequence(launchSpeed);
}

void ABaseIRMissile::LaunchSequence(float Speed) 
{
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	missileVelocity = Speed;
	isAir = true;
}

void ABaseIRMissile::activateSmoke()
{
	SmokeTrail = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		GetWorld(),
		SmokeTrailSystem,
		WeaponMesh->GetSocketLocation(TEXT("ExhaustSocket")),
		WeaponMesh->GetSocketRotation(TEXT("ExhaustSocket")),
		FVector(1.f),
		false,
		true
	);

	MissileRocket = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		GetWorld(),
		MissileRocketSystem,
		WeaponMesh->GetSocketLocation(TEXT("ExhaustSocket")),
		WeaponMesh->GetSocketRotation(TEXT("ExhaustSocket")),
		FVector(1.f),
		false,
		true
	);

	ProjectileMovement->InitialSpeed = missileVelocity <= 0 ? 1 : missileVelocity;
	ProjectileMovement->Activate();
}

void ABaseIRMissile::OnOverlapBegin(UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	CheckAndDelete(OtherActor);
}

void ABaseIRMissile::OnHit(UPrimitiveComponent* HitComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	FVector NormalImpulse,
	const FHitResult& Hit)
{
	CheckAndDelete(OtherActor);
}

void ABaseIRMissile::CheckAndDelete(AActor* OtherActor)
{
	if (!OtherActor || OtherActor == this || OtherActor == GetOwner() || !isAir) return;

	if (OtherActor->Implements<UTeamInterface>())
	{
		EFaction OtherFaction = Owner->Faction;
		OtherFaction = ITeamInterface::Execute_GetFaction(OtherActor);
		if (OtherFaction == Owner->Faction) return;
	}

	if (OtherActor->Implements<UDamageableInterface>())
	{
		IDamageableInterface::Execute_OnDamage(OtherActor, this, MissileStats->Damage);
	}

	DestroyMissile();
}

void ABaseIRMissile::DestroyMissile()
{
	if (SmokeTrail) SmokeTrail->Deactivate();
	if (MissileRocket) MissileRocket->Deactivate();
	Destroy();
}