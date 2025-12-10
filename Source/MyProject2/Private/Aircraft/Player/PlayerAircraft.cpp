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

	ManagerComp = CreateDefaultSubobject<UCameraManagerComponent>(TEXT("CameraManagerComponent"));
	AudioComp = CreateDefaultSubobject<UAircraftAudioComponent>(TEXT("AudioComponent"));

	PersonalAircraftAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("PersonalAircraftAudio"));
	PersonalAircraftAudio->SetupAttachment(GetRootComponent());
	PersonalAircraftAudio->bAutoActivate = false;
	PersonalAircraftAudio->bIsUISound = false;

	GunAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("GunAudio"));
	GunAudio->SetupAttachment(GetRootComponent());
	GunAudio->bAutoActivate = false;
	GunAudio->bIsUISound = false;

	health = 100;
	Faction = EFaction::Ally;
}

void APlayerAircraft::BeginPlay()
{
	Super::BeginPlay();
	//WeaponComponent->Setup(this, AirStats);
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
	Controlled->SetComponents(WeaponComponent);
}

void APlayerAircraft::HandleHit() 
{

}

void APlayerAircraft::WeaponComponentOnUnitDeath() {
	WeaponComponent->ResetLockedOn();
}

void APlayerAircraft::FireBullets() { if (WeaponComponent) WeaponComponent->FireBullets(); }

void APlayerAircraft::StartBullets() 
{
	GunSoundEffect(true);
	FireBullets();
	GetWorld()->GetTimerManager().SetTimer(RepeatTimerHandle, this, &APlayerAircraft::FireBullets, BulletStats->FireRate, true);
}

void APlayerAircraft::EndBullets()
{
	GunSoundEffect(false);
	GetWorld()->GetTimerManager().ClearTimer(RepeatTimerHandle);
}

void APlayerAircraft::SelectWeapon(float index) { if (WeaponComponent) WeaponComponent->SelectWeapon(index); }

int32 APlayerAircraft::AdvanceWeapon(int32 index, bool bForward) 
{
	if (!WeaponComponent || WeaponComponent->WeaponGroups.Num() == 0) return 0;

	TArray<TSubclassOf<ABaseWeapon>> Keys;
	WeaponComponent->WeaponGroups.GetKeys(Keys);

	int32 CurrentIndex = Keys.IndexOfByKey(WeaponComponent->GetWeapon()->GetClass());
	if (bForward) CurrentIndex = (CurrentIndex + 1) % Keys.Num();
	else CurrentIndex = (CurrentIndex - 1 + Keys.Num()) % Keys.Num();

	WeaponComponent->SelectWeapon(CurrentIndex);
	return CurrentIndex;
}

void APlayerAircraft::GunSoundEffect(bool bShooting) 
{
	if (AudioComp) AudioComp->HandleGunSound(bShooting);
}

void APlayerAircraft::CycleTarget() { if (RadarComponent) RadarComponent->CycleTarget(); };

void APlayerAircraft::SetHUD(APlayerHUD* InHUD) 
{
	if (!ManagerComp) return;
	ManagerComp->SetHUD(InHUD);
	ManagerComp->SetThirdPerson();
}

void APlayerAircraft::SwitchCameras() { if (ManagerComp) ManagerComp->SwitchCamera(); }

void APlayerAircraft::HandleVertical(float Vertical) { if (ManagerComp) ManagerComp->LookVer(Vertical); }

void APlayerAircraft::HandleHorizontal(float Horizontal) { if (ManagerComp) ManagerComp->LookHor(Horizontal); }