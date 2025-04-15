// Fill out your copyright notice in the Description page of Project Settings.


#include "R60.h"

//Initialize R60
AR60::AR60() 
{
	Missile = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Missile"));
	RootComponent = Missile;
	//Was being buggy without manually assigning Mesh, blueprint no work
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Game/Weapons/R-60M/R-60M_Final.R-60M_Final"));
	if (MeshAsset.Succeeded()) 
	{
		Missile->SetStaticMesh(MeshAsset.Object);
	}
	cooldownTime = 1.0;
}

void AR60::BeginPlay() 
{
	Super::BeginPlay();
}

void AR60::Fire() 
{
	if (GetOwner()) 
	{
		//"Push" the missile toward wherever is down from the aircraft orient

		FVector AircraftForward = GetOwner()->GetActorForwardVector();
		FVector AircraftUp = GetOwner()->GetActorUpVector();

		FVector LaunchDirection = AircraftForward + (AircraftUp * 0.5f);
		LaunchDirection.Normalize();
		DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

		Missile->SetSimulatePhysics(true);
		Missile->SetEnableGravity(false);
		Missile->AddImpulse(AircraftUp * -20.f, NAME_None, true);
		isAir = true;
	}
}