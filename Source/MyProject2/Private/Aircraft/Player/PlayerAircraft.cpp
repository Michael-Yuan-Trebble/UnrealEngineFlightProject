// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Player!"));
#include "Aircraft/Player/PlayerAircraft.h"
#include "AircraftPlayerController.h"
#include "FlightComponent.h"
#include "RadarComponent.h"
#include "WeaponSystemComponent.h"

APlayerAircraft::APlayerAircraft() 
{
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->bDoCollisionTest = false;
	SpringArm->SetupAttachment(Airframe, USpringArmComponent::SocketName);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);

	FlightComponent = CreateDefaultSubobject<UFlightComponent>(TEXT("FlightComponent"));
	WeaponComponent = CreateDefaultSubobject<UWeaponSystemComponent>(TEXT("WeaponComponent"));
	RadarComponent = CreateDefaultSubobject<URadarComponent>(TEXT("Radar"));
}

void APlayerAircraft::BeginPlay()
{
	Super::BeginPlay();
	FlightComponent->Setup(this, AirStats);
	WeaponComponent->Setup(this, AirStats);
	RadarComponent->Setup(this);
}

void APlayerAircraft::PossessedBy(AController* NewController) 
{
	Super::PossessedBy(NewController);

	Controlled = Cast<AAircraftPlayerController>(NewController);
	Controlled->FlightComp = FlightComponent;
	Controlled->WeaponComp = WeaponComponent;
	Controlled->RadarComp = RadarComponent;
}

void APlayerAircraft::SetStats(UAircraftStats* InStats)
{
	// Get Aircraft stats from UDataAssets

	AirStats = InStats;
	FlightComponent->AircraftStats = InStats;
}

USpringArmComponent* APlayerAircraft::GetSpringArm() const 
{
	return SpringArm;
}

UCameraComponent* APlayerAircraft::GetCamera() const
{
	return Camera;
}

float APlayerAircraft::ReturnSpringArmLength() const
{
	return springArmLength;
}

void APlayerAircraft::SetWeapons(TMap<FName, TSubclassOf<ABaseWeapon>> In) 
{
	WeaponComponent->Loadout = In;
	WeaponComponent->EquipWeapons();
}

UFlightComponent* APlayerAircraft::GetFlightComp() 
{
	return FlightComponent;
}