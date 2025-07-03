// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("R60!"));
#include "Weapons/Missiles/R60.h"

//Initialize R60
AR60::AR60() 
{
	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Missile"));
	RootComponent = WeaponMesh;

	//Was being buggy without manually assigning Mesh, blueprint no work
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Game/Weapons/R-60M/R-60M_Final.R-60M_Final"));
	if (MeshAsset.Succeeded()) 
	{
		WeaponMesh->SetStaticMesh(MeshAsset.Object);
	}
	cooldownTime = 1.0;
	range = 10000;
}

void AR60::BeginPlay() 
{
	Super::BeginPlay();
}

void AR60::Tick(float DeltaSeconds) 
{
	if (isAir) 
	{
		if (Tracking) 
		{
			//Missile is currently facing opposite direction it needs to

			FVector targetLocation = Tracking->GetActorLocation(); 
			FVector missileLocation = GetActorLocation();

			FVector directionToTarget = (targetLocation - missileLocation).GetSafeNormal();

			if (FVector::Dist(directionToTarget,FVector(5,5,5)) < 5) 
			{
				Destroy();
			}

			FRotator currentRotation = GetActorRotation();

			FRotator targetRotation = directionToTarget.Rotation();
			FRotator newRotation = FMath::RInterpTo(currentRotation, targetRotation, DeltaSeconds, turnRate);

			SetActorRotation(newRotation); 

			missileVelocity = FMath::Min(missileVelocity + missileAcceleration * DeltaSeconds, missileMaxSpeed);
			FVector vectorLocation = missileVelocity * GetActorForwardVector() * DeltaSeconds;
			AddActorWorldOffset(vectorLocation, true);
		}
		else 
		{
			missileVelocity = FMath::Min(missileVelocity + missileAcceleration * DeltaSeconds, missileMaxSpeed);
			FVector vectorLocation = missileVelocity * GetActorForwardVector() * DeltaSeconds;
			AddActorWorldOffset(vectorLocation, true);
		}

		timeTilDelt += DeltaSeconds;
		if (timeTilDelt >= timeDet) Destroy();
	}
}

//Not Selected
void AR60::FireStatic(float launchSpeed)
{
	LaunchSequence();
}

//Selected
void AR60::FireTracking(float launchSpeed, AActor* Target)
{
	LaunchSequence();
	Tracking = Target;
}

//Sequence for the missile to "drop" relative to any rotation and start firing operation
void AR60::LaunchSequence() 
{
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
}