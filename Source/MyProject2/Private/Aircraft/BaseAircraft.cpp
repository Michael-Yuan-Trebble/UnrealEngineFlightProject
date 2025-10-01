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
}

void ABaseAircraft::Tick(float DeltaTime)
{

	// Update cooldown timer

	for (int i = 0; i < AvailableWeapons.Num(); i++) 
	{
		if (AvailableWeapons[i].WeaponInstance && !AvailableWeapons[i].CanFire()) 
		{
			AvailableWeapons[i].UpdateCooldown(DeltaTime);
			//if (AvailableWeapons[i].CanFire()); //ReEquip(&AvailableWeapons[i]);
		}
	}
	UpdateLockedOn(DeltaTime);

	float ConeLength = 0;

	if (!CurrentWeapon) return; 
	ConeLength = CurrentWeapon->range;

	// Draw cone for aircraft's weapon lockon cone

	DrawDebugCone(
		GetWorld(),
		Airframe->GetComponentLocation(),
		Airframe->GetForwardVector(),
		ConeLength,
		FMath::DegreesToRadians(30 * 0.5f),
		FMath::DegreesToRadians(30 * 0.5f),
		12,
		FColor::Red,
		false,
		0.1,
		0
	);

	Super::Tick(DeltaTime);
}

void ABaseAircraft::AddPylons() 
{
	for (int i = 0; i < AirStats->NumOfPylons; i++)
	{
		UStaticMeshComponent* TempPylon = NewObject<UStaticMeshComponent>(this);
		FName SocketName = FName(*FString::Printf(TEXT("Pylon%d"), i));
		if (TempPylon && AirStats->Pylon)
		{
			TempPylon->SetupAttachment(Airframe, SocketName);
			TempPylon->RegisterComponent();
			TempPylon->SetStaticMesh(AirStats->Pylon);
			TempPylon->SetRelativeLocation(FVector(0,0,-20));
			PylonSockets.Add(SocketName, TempPylon);
		}
	}
}

void ABaseAircraft::UpdateLockedOn(float DeltaSeconds) 
{
	// Update timer on missile lock

	if (!Tracking) return;

	FVector ToTarget = Tracking->GetActorLocation() - GetActorLocation();
	float Distance = ToTarget.Size();
	ToTarget.Normalize();

	float Dot = FVector::DotProduct(Airframe->GetForwardVector(), ToTarget);
	bool bInRange = false;
	if (CurrentWeapon) bInRange = Distance <= CurrentWeapon->range;
	bool bInCone = Dot > FMath::Cos(FMath::DegreesToRadians(30.f));
	if (bInRange && bInCone) 
	{
		LockTime += DeltaSeconds;
		bLocked = LockTime >= 1 ? true : false;
	}
	else 
	{
		bLocked = false;
		LockTime = 0.f;
	} 
	
}

FVector ABaseAircraft::GetTargetLocation() const {return this->GetActorLocation();}

bool ABaseAircraft::IsLockable() const {return isAlive;}

float ABaseAircraft::ReturnTakeoffSpeed() const {return TakeoffSpeed;}

USkeletalMeshComponent* ABaseAircraft::GetMesh() const {return Airframe;}