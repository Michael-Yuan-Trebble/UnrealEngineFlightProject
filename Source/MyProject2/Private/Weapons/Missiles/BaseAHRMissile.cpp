// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Hit!"));
#include "Weapons/Missiles/BaseAHRMissile.h"
#include "NiagaraFunctionLibrary.h"
#include "Aircraft/BaseAircraft.h"
#include "Structs and Data/DamageableInterface.h"
#include "Structs and Data/TeamInterface.h"

ABaseAHRMissile::ABaseAHRMissile()
{
	PrimaryActorTick.bCanEverTick = true;
	timeTilDelt = 0;
	isAir = false;
	isDropPhase = false;
	canLock = true;
	Collision = CreateDefaultSubobject<UBoxComponent>(TEXT("Missile Collision"));
	RootComponent = Collision;

	Collision->SetCollisionProfileName(TEXT("Projectile"));
	Collision->SetNotifyRigidBodyCollision(true);
	Collision->SetGenerateOverlapEvents(false);

	Collision->OnComponentBeginOverlap.AddDynamic(this, &ABaseAHRMissile::OnOverlapBegin);
	Collision->OnComponentEndOverlap.AddDynamic(this, &ABaseAHRMissile::OnEndOverlap);
	Collision->OnComponentHit.AddDynamic(this, &ABaseAHRMissile::OnHit);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->InitialSpeed = 0;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = 0.f;

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Missile"));
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponMesh->SetupAttachment(RootComponent);
}

void ABaseAHRMissile::BeginPlay() 
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

void ABaseAHRMissile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!Owner || !Owner->IsValidLowLevelFast()) return;

	if (!isAir) return;
	if (isDropPhase)
	{
		DropTimer += DeltaTime;

		// Drop Sequence: "Launch" downwards
		dropVelocity += DropAcceleration * DeltaTime;
		FVector DropMove = -Owner->GetActorUpVector() * dropVelocity;
		FVector Forward = Owner->GetActorForwardVector() * missileVelocity * DeltaTime;
		FVector TotalMove = DropMove + Forward;

		AddActorWorldOffset(TotalMove, true);
		isDropPhase = DropTimer < 0.2;
		return;
	}

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

	// Missile explodes at range

	if (!(timeTilDelt >= MissileStats->LifeTime)) return;

	DestroyMissile();
}

void ABaseAHRMissile::FireStatic(float launchSpeed)
{
	LaunchSequence(launchSpeed);
}

void ABaseAHRMissile::FireTracking(float launchSpeed, AActor* Target)
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

void ABaseAHRMissile::LaunchSequence(float Speed)
{
	// Unbound Missile from Socket

	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	missileVelocity = Speed;
	isAir = true;
	isDropPhase = true;

	ProjectileMovement->StopMovementImmediately();
	ProjectileMovement->SetUpdatedComponent(Collision);
	ProjectileMovement->InitialSpeed = FMath::Max(1.f,missileVelocity);
	ProjectileMovement->Activate(true);
}

void ABaseAHRMissile::activateSmoke() 
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

void ABaseAHRMissile::OnOverlapBegin(UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult) 
{
	CheckAndDelete(OtherActor);
}

void ABaseAHRMissile::OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
	return;
}

void ABaseAHRMissile::OnHit(UPrimitiveComponent* HitComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	FVector NormalImpulse,
	const FHitResult& Hit) 
{
	CheckAndDelete(OtherActor);
}

void ABaseAHRMissile::CheckAndDelete(AActor* OtherActor) 
{
	if (!OtherActor || OtherActor == this || OtherActor == GetOwner() || !isAir) return;

	if (!Owner || !Owner->IsValidLowLevelFast()) return;

	if (OtherActor->Implements<UTeamInterface>())
	{
		EFaction OtherFaction = Owner->Faction;
		OtherFaction = ITeamInterface::Execute_GetFaction(OtherActor);
		if (OtherFaction == Owner->Faction) return;
	}

	if (OtherActor->Implements<UDamageableInterface>())
	{
		IDamageableInterface::Execute_OnHitByMissile(OtherActor, this, MissileStats->Damage);
	}

	DestroyMissile();
}

bool bDestroyed = false;

void ABaseAHRMissile::DestroyMissile() 
{

	if (bDestroyed) return;
	bDestroyed = true;

	if (SmokeTrail) SmokeTrail->Deactivate();
	if (MissileRocket) MissileRocket->Deactivate();
	Destroy();
}

float ABaseAHRMissile::ReturnCooldownTime() { return cooldownTime; }