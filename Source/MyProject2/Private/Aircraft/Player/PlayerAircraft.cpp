// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Player!"));
#include "Aircraft/Player/PlayerAircraft.h"
#include "AircraftPlayerController.h"
#include "Aircraft/FlightComponent.h"
#include "Aircraft/RadarComponent.h"
#include "HealthComponent.h"
#include "Aircraft/AircraftAudioComponent.h"
#include "Aircraft/WeaponSystemComponent.h"
#include "Components/AudioComponent.h"
#include "Aircraft/Player/CameraManagerComponent.h"

APlayerAircraft::APlayerAircraft() 
{
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->bDoCollisionTest = false;
	SpringArm->SetupAttachment(Airframe, USpringArmComponent::SocketName);

	ThirdPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ThirdPersonCamera"));
	ThirdPersonCamera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);

	FirstPersonSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("FirstPersonPivot"));
	FirstPersonSpringArm->SetupAttachment(Airframe, USpringArmComponent::SocketName);

	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCamera->SetupAttachment(FirstPersonSpringArm);

	WeaponComponent = CreateDefaultSubobject<UWeaponSystemComponent>(TEXT("WeaponComponent"));
	ManagerComp = CreateDefaultSubobject<UCameraManagerComponent>(TEXT("CameraManagerComponent"));
	AudioComp = CreateDefaultSubobject<UAircraftAudioComponent>(TEXT("AudioComponent"));

	PersonalAircraftAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("PersonalAircraftAudio"));
	PersonalAircraftAudio->SetupAttachment(GetRootComponent());
	PersonalAircraftAudio->bAutoActivate = false;
	PersonalAircraftAudio->bIsUISound = false;

	health = 100;
	Faction = EFaction::Ally;
}

void APlayerAircraft::BeginPlay()
{
	Super::BeginPlay();
	WeaponComponent->Setup(this, AirStats);
	AudioComp->SetControlled(this);
	ManagerComp->SetSpringArm(SpringArm);
	ManagerComp->SetControlled(this);
	ManagerComp->SetAudioComp(AudioComp);
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
	Controlled->SetComponents(FlightComponent, WeaponComponent, RadarComponent, ManagerComp);
}

void APlayerAircraft::HandleHit() 
{

}