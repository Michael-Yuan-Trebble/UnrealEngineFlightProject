// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Hit!"));
#include "Weapons/Missiles/BaseAHRMissile.h"
#include "NiagaraFunctionLibrary.h"
#include "Units/Aircraft/BaseAircraft.h"
#include "Interfaces/DamageableInterface.h"
#include "Interfaces/TeamInterface.h"
#include "Interfaces/ApproachingMissileInterface.h"

ABaseAHRMissile::ABaseAHRMissile()
{
	PrimaryActorTick.bCanEverTick = true;
	canLock = true;
}

void ABaseAHRMissile::BeginPlay() 
{
	Super::BeginPlay();
	if (!MissileStats) return;
	WeaponName = MissileStats->InGameName;
	missileAcceleration = MissileStats->Acceleration;
	missileMaxSpeed = MissileStats->MaxSpeed;
	cooldownTime = MissileStats->Cooldown;
	range = MissileStats->LockOnRange;
	turnRate = MissileStats->TurnRate;
	ProjectileMovement->MaxSpeed = MissileStats->MaxSpeed;
	SupportedTargetTypes = MissileStats->SupportedTargetTypes;
}

#define DROPTIME 0.2f

void ABaseAHRMissile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!Owner || !Owner->IsValidLowLevelFast()) return;

	if (!bAir) return;
	if (isDropPhase)
	{
		DropTimer += DeltaTime;

		// Drop Sequence: "Launch" downwards
		dropVelocity += DropAcceleration * DeltaTime;
		FVector DropMove = -Owner->GetActorUpVector() * dropVelocity;
		FVector Forward = Owner->GetActorForwardVector() * missileVelocity * DeltaTime;
		FVector TotalMove = DropMove + Forward;

		AddActorWorldOffset(TotalMove, true);
		isDropPhase = DropTimer < DROPTIME;
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

	if (IsValid(Tracking))
	{
		if (CalculateIfOvershoot(Tracking->GetActorLocation() - GetActorLocation())) 
		{
			OnWeaponResult.Broadcast(false);
			ProjectileMovement->HomingTargetComponent = nullptr;
			ProjectileMovement->bIsHomingProjectile = false;
			bMissed = true;

			if (ABaseAircraft* Aircraft = Cast<ABaseAircraft>(Tracking))
			{
				if (Aircraft->Implements<UApproachingMissileInterface>())
				{
					IApproachingMissileInterface::Execute_UnregisterIncomingMissile(Aircraft,this);
				}
			}
		}
	}

	// Missile explodes at range

	if (!(timeTilDelt >= MissileStats->LifeTime)) return;
	if (!bMissed) OnWeaponResult.Broadcast(false);
	DestroyMissile();
}

void ABaseAHRMissile::FireStatic(float launchSpeed)
{
	LaunchSequence(launchSpeed);
}

void ABaseAHRMissile::FireTracking(float launchSpeed, AActor* Target)
{
	Tracking = Target;
	if (IsValid(Tracking))
	{
		ProjectileMovement->bIsHomingProjectile = true;
		ProjectileMovement->HomingTargetComponent = Tracking->GetRootComponent();
		ProjectileMovement->HomingAccelerationMagnitude = turnRate;
		if (ABaseAircraft* Aircraft = Cast<ABaseAircraft>(Tracking))
		{
			Aircraft->OnMissileLaunchedAtSelf.Broadcast(this);
			if (Aircraft->Implements<UApproachingMissileInterface>()) 
			{
				IApproachingMissileInterface::Execute_RegisterIncomingMissile(Aircraft,this);
			}
		}
	}
	LaunchSequence(launchSpeed);
}

void ABaseAHRMissile::LaunchSequence(float Speed)
{
	// Unbound Missile from Socket

	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	missileVelocity = Speed;
	bAir = true;
	isDropPhase = true;

	ProjectileMovement->SetUpdatedComponent(Collision);
	ProjectileMovement->InitialSpeed = FMath::Max(1.f,missileVelocity);
	ProjectileMovement->Activate(true);
}

void ABaseAHRMissile::CheckAndDelete(AActor* OtherActor) 
{
	if (!OtherActor || OtherActor == this || OtherActor == Owner || !bAir) return;
	if (!Owner || !Owner->IsValidLowLevelFast()) return;

	if (IsValid(Tracking)) {
		if (ABaseAircraft* Aircraft = Cast<ABaseAircraft>(Tracking))
		{
			if (Aircraft->Implements<UApproachingMissileInterface>())
			{
				IApproachingMissileInterface::Execute_UnregisterIncomingMissile(Aircraft,this);
			}
		}
	}

	if (OtherActor->Implements<UTeamInterface>())
	{
		EFaction OtherFaction = Owner->Faction;
		OtherFaction = ITeamInterface::Execute_GetFaction(OtherActor);
		if (OtherFaction == Owner->Faction) return;
	}

	if (OtherActor->Implements<UDamageableInterface>())
	{
		IDamageableInterface::Execute_OnDamage(OtherActor, this, Owner, OtherActor, MissileStats->Damage);
		OnWeaponResult.Broadcast(true);
	}
	else OnWeaponResult.Broadcast(false);

	DestroyMissile();
}
