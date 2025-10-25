// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Base!"));
#include "Aircraft/BaseAircraft.h"
#include "AircraftRegistry.h"
#include "Aircraft/FlightComponent.h"
#include "Aircraft/WeaponSystemComponent.h"
#include "Aircraft/RadarComponent.h"

ABaseAircraft::ABaseAircraft()
{
	Collision = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));

	SetRootComponent(Collision);

	Collision->SetCollisionProfileName(TEXT("Pawn"));
	Collision->SetNotifyRigidBodyCollision(true);
	Collision->SetGenerateOverlapEvents(true);
	Collision->SetBoxExtent(FVector(30.f));

	Airframe = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Airframe"));
	Airframe->SetupAttachment(Collision);
	Airframe->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	BodyCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BodyCollision"));
	BodyCollision->SetupAttachment(Collision);
	BodyCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	LeftWingCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftWingCollision"));
	LeftWingCollision->SetupAttachment(Collision);
	LeftWingCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	RightWingCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("RightWingCollision"));
	RightWingCollision->SetupAttachment(Collision);
	RightWingCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	RudderCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("RudderCollision"));
	RudderCollision->SetupAttachment(Collision);
	RudderCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	FlightComponent = CreateDefaultSubobject<UFlightComponent>(TEXT("FlightComponent"));
	RadarComponent = CreateDefaultSubobject<URadarComponent>(TEXT("Radar"));

	PrimaryActorTick.bCanEverTick = true;
	bLocked = false;
}

void ABaseAircraft::BeginPlay()
{
	Super::BeginPlay();

	RadarComponent->Setup(this);
	FlightComponent->Setup(this, AirStats);

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
}

void ABaseAircraft::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABaseAircraft::OnHitByMissile_Implementation(AActor* Missile, float Damage)
{
	health -= Damage;
	if (health <= 0.f) 
	{
		Destroy();
	}
}