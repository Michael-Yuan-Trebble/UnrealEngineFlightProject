// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Missile!"));
#include "Weapons/Missiles/BaseIRMissile.h"
#include "NiagaraFunctionLibrary.h"

ABaseIRMissile::ABaseIRMissile()
{
	PrimaryActorTick.bCanEverTick = true;

	timeDet = 5;
	timeTilDelt = 0;
	isAir = false;
	isDropPhase = false;
	canLock = true;
}

void ABaseIRMissile::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseIRMissile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!isAir) return;

	// VFX
	if (SmokeTrail) 
	{
		SmokeTrail->SetWorldLocation(WeaponMesh->GetSocketLocation(TEXT("ExhaustSocket")));
		SmokeTrail->SetWorldRotation(WeaponMesh->GetSocketRotation(TEXT("ExhaustSocket")));
	}

	// ====================================
	// Missile is Deployed
	// Initial drop movement and transition into pathing
	// ====================================

	if (isDropPhase) 
	{
		DropTimer += DeltaTime;

		// Drop Sequence: "Launch" downwards

		FVector DropMove = -GetOwner()->GetActorUpVector() * 600.f * DeltaTime;
		FVector Forward = GetOwner()->GetActorForwardVector() * missileVelocity * DeltaTime;
		FVector TotalMove = DropMove + Forward;
		
		AddActorWorldOffset(TotalMove, true);
		if (DropTimer >= 0.1) {
			isDropPhase = false;

		}
		return;
	}

	missileVelocity += missileAcceleration * DeltaTime;
	missileVelocity = FMath::Clamp(missileVelocity, 0.f, missileMaxSpeed);

	// Movement Forward

	FVector DeltaMove = GetActorForwardVector() * missileVelocity * DeltaTime;
	AddActorWorldOffset(DeltaMove, true);

	timeTilDelt += DeltaTime;

	// Missile explodes at range

	if (!(timeTilDelt >= timeDet)) return;

	if (SmokeTrail) {
		SmokeTrail->Deactivate();
	}

	Destroy();
}

void ABaseIRMissile::FireStatic(float launchSpeed)
{
	LaunchSequence(launchSpeed);
}

void ABaseIRMissile::FireTracking(float launchSpeed, AActor* Target) 
{
	Tracking = Target;
	LaunchSequence(launchSpeed);
}

void ABaseIRMissile::LaunchSequence(float Speed) 
{
	if (GetOwner())
	{
		// ====================================
		// Unbound Missile from Socket
		// ====================================

		DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		WeaponMesh->SetCollisionObjectType(ECC_PhysicsBody);
		WeaponMesh->SetCollisionResponseToAllChannels(ECR_Block);
		WeaponMesh->SetCollisionProfileName(TEXT("PhysicsActor"));
		missileVelocity = Speed;
		isAir = true;
		isDropPhase = true;

		SmokeTrail = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			SmokeTrailSystem,
			WeaponMesh->GetSocketLocation(TEXT("ExhaustSocket")),
			WeaponMesh->GetSocketRotation(TEXT("ExhaustSocket")),
			FVector(1.f),
			false,
			true
		);
	}
}

float ABaseIRMissile::ReturnCooldownTime() 
{
	return cooldownTime;
}