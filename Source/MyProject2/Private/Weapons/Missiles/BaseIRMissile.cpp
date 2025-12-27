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
}

void ABaseIRMissile::BeginPlay()
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

void ABaseIRMissile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!Owner || !Owner->IsValidLowLevel()) return;

	if (!bAir) return;
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
		}
		if (ABaseAircraft* Aircraft = Cast<ABaseAircraft>(Tracking))
		{
			if (Aircraft->Implements<UApproachingMissileInterface>()) 
			{
				IApproachingMissileInterface::Execute_UnregisterIncomingMissile(Aircraft,this);
			}
		}
	}

	// Missile explodes at range

	if (!(timeTilDelt >= MissileStats->LifeTime)) return;
	if (!bMissed) OnWeaponResult.Broadcast(false);
	DestroyMissile();
}

void ABaseIRMissile::FireStatic(float launchSpeed)
{
	LaunchSequence(launchSpeed);
}

void ABaseIRMissile::FireTracking(float launchSpeed, AActor* Target) 
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

void ABaseIRMissile::LaunchSequence(float Speed) 
{
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	missileVelocity = Speed;
	bAir = true;

	ProjectileMovement->SetUpdatedComponent(Collision);
	ProjectileMovement->InitialSpeed = FMath::Max(1.f, missileVelocity);
	ProjectileMovement->Activate(true);
}

void ABaseIRMissile::CheckAndDelete(AActor* OtherActor)
{
	if (!OtherActor || OtherActor == this || OtherActor == Owner || !bAir) return;

	if (IsValid(Tracking))
	{
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