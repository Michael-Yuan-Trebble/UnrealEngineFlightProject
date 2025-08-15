// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapons/Missiles/R60.h"

AR60::AR60() 
{
	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Missile"));
	RootComponent = WeaponMesh;

	WeaponName = "R-60";
	missileAcceleration = 1000.f;
	missileMaxSpeed = 1000.f;
	turnRate = 50.f;

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
	Super::Tick(DeltaSeconds);
}