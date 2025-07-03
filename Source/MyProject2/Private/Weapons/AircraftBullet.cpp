// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Bullet!"));
#include "Weapons/AircraftBullet.h"
#include "Components/BoxComponent.h"
#include "DrawDebugHelpers.h"

AAircraftBullet::AAircraftBullet()
{
	PrimaryActorTick.bCanEverTick = true;

	BulletMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BulletMesh"));
	RootComponent = BulletMesh;

	BulletMesh->SetWorldScale3D(FVector(5.f));

	Collision = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
	Collision->SetupAttachment(BulletMesh);

	BulletMesh->SetSimulatePhysics(true);
	BulletMesh->SetEnableGravity(true);
	BulletMesh->SetVisibility(true);
	BulletMesh->SetCollisionProfileName(TEXT("BlockAll"));
}

void AAircraftBullet::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAircraftBullet::Tick(float DeltaTime)
{
	if (currentTime >= LifeTime) 
	{
		print(text)
		Destroy();
	}
	currentTime += DeltaTime;

	Super::Tick(DeltaTime);

}

void AAircraftBullet::FireInDirection(const FVector& ShootDirection) 
{
	BulletMesh->AddImpulse(ShootDirection * BulletSpeed, NAME_None, true);
}

