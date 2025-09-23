// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Missiles/Aim9.h"

AAim9::AAim9() {
	PrimaryActorTick.bCanEverTick = true;
	Collision = CreateDefaultSubobject<UBoxComponent>(TEXT("Missile Collision"));
	RootComponent = Collision;

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Missile"));
	WeaponMesh->SetupAttachment(RootComponent);

	WeaponName = "Aim-9";
	missileAcceleration = 10000.f;
	missileMaxSpeed = 30000;
	turnRate = 50.f;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Game/Weapons/Aim-9L/Aim-9L_LOD0.Aim-9L_LOD0"));
	if (MeshAsset.Succeeded()) 
	{
		WeaponMesh->SetStaticMesh(MeshAsset.Object);
	}
	cooldownTime = 1.0;
	range = 1000000;
}

void AAim9::BeginPlay() {
	Super::BeginPlay();
}

void AAim9::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}