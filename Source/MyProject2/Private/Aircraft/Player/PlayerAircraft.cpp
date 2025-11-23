// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Player!"));
#include "Aircraft/Player/PlayerAircraft.h"
#include "AircraftPlayerController.h"
#include "Aircraft/FlightComponent.h"
#include "Aircraft/RadarComponent.h"
#include "Aircraft/WeaponSystemComponent.h"
#include "Aircraft/Player/CameraManagerComponent.h"

APlayerAircraft::APlayerAircraft() 
{

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->bDoCollisionTest = false;
	SpringArm->SetupAttachment(Airframe, USpringArmComponent::SocketName);

	ThirdPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ThirdPersonCamera"));
	ThirdPersonCamera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);

	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCamera->SetupAttachment(RootComponent);

	WeaponComponent = CreateDefaultSubobject<UWeaponSystemComponent>(TEXT("WeaponComponent"));
	ManagerComp = CreateDefaultSubobject<UCameraManagerComponent>(TEXT("CameraManagerComponent"));

	health = 100;
	Faction = EFaction::Ally;
}

void APlayerAircraft::BeginPlay()
{
	Super::BeginPlay();
	WeaponComponent->Setup(this, AirStats);
	ManagerComp->SetSpringArm(SpringArm);
	ManagerComp->SetControlled(this);

	FirstPersonCamera->SetActive(false);
	ThirdPersonCamera->SetActive(true);
}

void APlayerAircraft::Tick(float DeltaSeconds) 
{
	Super::Tick(DeltaSeconds);
}

void APlayerAircraft::PossessedBy(AController* NewController) 
{
	Super::PossessedBy(NewController);

	Controlled = Cast<AAircraftPlayerController>(NewController);
	if (!Controlled) return;
	Controlled->FlightComp = FlightComponent;
	Controlled->WeaponComp = WeaponComponent;
	Controlled->RadarComp = RadarComponent;
	Controlled->ManagerComp = ManagerComp;
	Controlled->SetViewTargetWithBlend(this, 0.3f, VTBlend_Cubic);
}