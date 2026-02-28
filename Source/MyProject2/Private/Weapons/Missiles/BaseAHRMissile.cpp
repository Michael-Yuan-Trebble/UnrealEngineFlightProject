// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Hit!"));
#include "Weapons/Missiles/BaseAHRMissile.h"
#include "NiagaraFunctionLibrary.h"
#include "Units/Aircraft/BaseAircraft.h"
#include "Interfaces/DamageableInterface.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "Interfaces/TeamInterface.h"
#include "Components/BoxComponent.h"
#include "Interfaces/ApproachingMissileInterface.h"

ABaseAHRMissile::ABaseAHRMissile()
{
	PrimaryActorTick.bCanEverTick = true;
	canLock = true;
}

void ABaseAHRMissile::BeginPlay() 
{
	Super::BeginPlay();

	UARHMissileStats* LoadedStats = MissileStats.LoadSynchronous();

	if (!IsValid(LoadedStats)) return;
	WeaponName = LoadedStats->InGameName;
	missileAcceleration = LoadedStats->Acceleration;
	missileMaxSpeed = LoadedStats->MaxSpeed;
	cooldownTime = LoadedStats->Cooldown;
	range = LoadedStats->LockOnRange;
	turnRate = LoadedStats->TurnRate;
	damage = LoadedStats->Damage;
	ProjectileMovement->MaxSpeed = LoadedStats->MaxSpeed;
	SupportedTargetTypes = LoadedStats->SupportedTargetTypes;
}

void ABaseAHRMissile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bAir) return;
	if (isDropPhase)
	{
		if (ABaseAircraft* LoadedAircraft = AircraftOwner.Get()) {
			DropTimer += DeltaTime;

			// Drop Sequence: "Launch" downwards
			dropVelocity += DropAcceleration * DeltaTime;
			FVector DropMove = -LoadedAircraft->GetActorUpVector() * dropVelocity;
			FVector Forward = LoadedAircraft->GetActorForwardVector() * missileVelocity * DeltaTime;
			FVector TotalMove = DropMove + Forward;

			AddActorWorldOffset(TotalMove, true);
			isDropPhase = DropTimer < DropTime;
			return;
		}
	}

	if (!SmokeTrail.IsValid() || !MissileRocket.IsValid()) activateSmoke();

	ProjectileMovement->Velocity += GetActorForwardVector() * missileAcceleration * DeltaTime;
	ProjectileMovement->Velocity = ProjectileMovement->Velocity.GetClampedToMaxSize(ProjectileMovement->MaxSpeed);

	if (SmokeTrail.IsValid())
	{
		SmokeTrail->SetWorldLocation(WeaponMesh->GetSocketLocation(TEXT("ExhaustSocket")));
		SmokeTrail->SetWorldRotation(WeaponMesh->GetSocketRotation(TEXT("ExhaustSocket")));
	}

	if (MissileRocket.IsValid())
	{
		MissileRocket->SetWorldLocation(WeaponMesh->GetSocketLocation(TEXT("ExhaustSocket")));
		MissileRocket->SetWorldRotation(WeaponMesh->GetSocketRotation(TEXT("ExhaustSocket")));
	}

	timeTilDelt += DeltaTime;

	if (AActor* LoadedTracking = Tracking.Get())
	{
		if (CalculateIfOvershoot(LoadedTracking->GetActorLocation() - GetActorLocation()))
		{
			OnWeaponResult.Broadcast(false);
			ProjectileMovement->HomingTargetComponent = nullptr;
			ProjectileMovement->bIsHomingProjectile = false;
			bMissed = true;

			if (ABaseAircraft* Aircraft = Cast<ABaseAircraft>(LoadedTracking))
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
	if (Target)
	{
		ProjectileMovement->bIsHomingProjectile = true;
		ProjectileMovement->HomingTargetComponent = Target->GetRootComponent();
		ProjectileMovement->HomingAccelerationMagnitude = turnRate;
		if (ABaseAircraft* Aircraft = Cast<ABaseAircraft>(Target))
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
	if (!bAir || !OtherActor || OtherActor == this || !AircraftOwner.IsValid() || OtherActor == AircraftOwner.Get()) return;

	if (AActor* LoadedTracking = Tracking.Get()) {
		if (ABaseAircraft* Aircraft = Cast<ABaseAircraft>(LoadedTracking))
		{
			if (Aircraft->Implements<UApproachingMissileInterface>())
			{
				IApproachingMissileInterface::Execute_UnregisterIncomingMissile(Aircraft, this);
			}
		}
	}

	if (ABaseAircraft* LoadedOwner = AircraftOwner.Get()) {
		if (OtherActor->Implements<UTeamInterface>())
		{
			EFaction OtherFaction = LoadedOwner->GetFaction();
			OtherFaction = ITeamInterface::Execute_GetFaction(OtherActor);
			if (OtherFaction == LoadedOwner->GetFaction()) return;
		}

		if (OtherActor->Implements<UDamageableInterface>())
		{
			IDamageableInterface::Execute_OnDamage(OtherActor, this, LoadedOwner, OtherActor, damage);
			OnWeaponResult.Broadcast(true);
		}
		else OnWeaponResult.Broadcast(false);
	}

	DestroyMissile();
}
