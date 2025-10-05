// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Base!"));
#include "Aircraft/BaseAircraft.h"
#include "Engine/SkeletalMeshSocket.h"
#include "DrawDebugHelpers.h"
#include "Structs and Data/CooldownWeapon.h"
#include "Weapons/AircraftBullet.h"
#include "AircraftRegistry.h"
#include "Aircraft/FlightComponent.h"
#include "Aircraft/WeaponSystemComponent.h"
#include "Aircraft/RadarComponent.h"
#include "AircraftPlayerController.h"

ABaseAircraft::ABaseAircraft()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(RootComponent);
	Airframe = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Airframe"));
	Airframe->SetupAttachment(RootComponent);

	Collision = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
	Collision->SetupAttachment(Airframe);

	BodyCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BodyCollision"));
	BodyCollision->SetupAttachment(Collision);

	LeftWingCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftWingCollision"));
	LeftWingCollision->SetupAttachment(Collision);

	RightWingCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("RightWingCollision"));
	RightWingCollision->SetupAttachment(Collision);

	RudderCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("RudderCollision"));
	RudderCollision->SetupAttachment(Collision);

	FlightComponent = CreateDefaultSubobject<UFlightComponent>(TEXT("FlightComponent"));
	RadarComponent = CreateDefaultSubobject<URadarComponent>(TEXT("Radar"));

	PrimaryActorTick.bCanEverTick = true;
	bLocked = false;
}

void ABaseAircraft::BeginPlay()
{
	Super::BeginPlay();

	// Update world registry with each aircraft spawned, caching all aircraft fielded

	if (UWorld* World = GetWorld()) 
	{
		if (AAircraftRegistry* Registry = AAircraftRegistry::Get(World)) 
		{
			Registry->Register(this);
		}
	}

}

void ABaseAircraft::EndPlay(const EEndPlayReason::Type EndPlayReason) 
{

	// Safely unregister all aircraft from cache

	if (UWorld* World = GetWorld()) 
	{
		if (AAircraftRegistry* Registry = AAircraftRegistry::Get(World)) 
		{
			Registry->Unregister(this);
		}
	}
	Super::EndPlay(EndPlayReason);
}

void ABaseAircraft::PossessedBy(AController* NewController) 
{
	Super::PossessedBy(NewController);

	RadarComponent->Setup(this);
	FlightComponent->Setup(this, AirStats);

}

void ABaseAircraft::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

FVector ABaseAircraft::GetTargetLocation() const {return this->GetActorLocation();}

bool ABaseAircraft::IsLockable() const {return isAlive;}

float ABaseAircraft::ReturnTakeoffSpeed() const {return TakeoffSpeed;}

USkeletalMeshComponent* ABaseAircraft::GetMesh() const {return Airframe;}