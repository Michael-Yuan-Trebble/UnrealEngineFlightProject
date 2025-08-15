// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Missile!"));
#include "Weapons/Missiles/BaseIRMissile.h"

ABaseIRMissile::ABaseIRMissile()
{
	PrimaryActorTick.bCanEverTick = true;

	timeDet = 5;
	timeTilDelt = 0;
	isAir = false;
}

void ABaseIRMissile::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseIRMissile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!isAir) return;
	timeTilDelt += DeltaTime;

	if (!(timeTilDelt >= timeDet)) return;
	Destroy();
}

void ABaseIRMissile::FireStatic(float launchSpeed)
{
	LaunchSequence();
}

void ABaseIRMissile::FireTracking(float launchSpeed, AActor* Target) 
{
	Tracking = Target;
	LaunchSequence();
}

void ABaseIRMissile::LaunchSequence() {
	/*
	
		if (GetOwner())
	{
		FVector AircraftForward = GetOwner()->GetActorForwardVector();
		FVector AircraftUp = GetOwner()->GetActorUpVector();

		FVector LaunchDirection = AircraftForward + (AircraftUp * 0.5f);
		LaunchDirection.Normalize();
		DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		WeaponMesh->SetCollisionObjectType(ECC_PhysicsBody);
		WeaponMesh->SetCollisionResponseToAllChannels(ECR_Block);
		WeaponMesh->SetCollisionProfileName(TEXT("PhysicsActor"));
		WeaponMesh->SetSimulatePhysics(true);
		WeaponMesh->SetEnableGravity(false);
		//Missile->AddImpulse(AircraftUp * -20.f, NAME_None, true);
		missileVelocity = currentSpeed;
		isAir = true;
	}
	
	*/
}

float ABaseIRMissile::ReturnCooldownTime() 
{
	return cooldownTime;
}