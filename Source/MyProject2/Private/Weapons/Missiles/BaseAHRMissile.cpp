// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Hit!"));
#include "Weapons/Missiles/BaseAHRMissile.h"
#include "NiagaraFunctionLibrary.h"
#include "Structs and Data/DamageableInterface.h"

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
	Collision->IgnoreActorWhenMoving(GetOwner(), true);
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
	if (!isAir) return;
	if (isDropPhase) 
	{
		DropTimer += DeltaTime;

		// Drop Sequence: "Launch" downwards
		dropVelocity += DropAcceleration * DeltaTime;
		FVector DropMove = -GetOwner()->GetActorUpVector() * dropVelocity;
		FVector Forward = GetOwner()->GetActorForwardVector() * missileVelocity * DeltaTime;
		FVector TotalMove = DropMove + Forward;

		AddActorWorldOffset(TotalMove, true);
		isDropPhase = DropTimer < 0.2;
		return;
	}

	if (!SmokeTrail || !MissileRocket)
	{
		activateSmoke();
	}

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

	if (SmokeTrail) SmokeTrail->Deactivate();
	if (MissileRocket) MissileRocket->Deactivate();

	Destroy();
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
		// ====================================
		// Unbound Missile from Socket
		// ====================================

	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	missileVelocity = Speed;
	isAir = true;
	isDropPhase = true;
}

void ABaseAHRMissile::activateSmoke() 
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

void ABaseAHRMissile::OnOverlapBegin(UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult) {
	if (!OtherActor || OtherActor == this || OtherActor == GetOwner())
		return;
	if (!isAir) return;

	if (OtherActor->Implements<UDamageableInterface>()) 
	{
		IDamageableInterface::Execute_OnHitByMissile(OtherActor, this, MissileStats->Damage);
	}

	if (SmokeTrail) SmokeTrail->Deactivate();
	if (MissileRocket) MissileRocket->Deactivate();

	Destroy();
}

float ABaseAHRMissile::ReturnCooldownTime() { return cooldownTime; }