// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Player!"));
#include "Units/Aircraft/Player/PlayerAircraft.h"
#include "Player Info/AircraftPlayerController.h"
#include "Units/Aircraft/FlightComponent.h"
#include "Units/Aircraft/RadarComponent.h"
#include "Units/HealthComponent.h"
#include "Units/Aircraft/AircraftAudioComponent.h"
#include "Units/Aircraft/WeaponSystemComponent.h"
#include "Components/AudioComponent.h"
#include "Units/Aircraft/Player/CameraManagerComponent.h"

APlayerAircraft::APlayerAircraft() 
{
	ThirdPersonSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("ThirdPersonPivot"));
	ThirdPersonSpringArm->bDoCollisionTest = false;
	ThirdPersonSpringArm->SetupAttachment(Airframe, USpringArmComponent::SocketName);
	ThirdPersonSpringArm->bEnableCameraLag = false;
	ThirdPersonSpringArm->bEnableCameraRotationLag = true;
	ThirdPersonSpringArm->CameraRotationLagSpeed = 3.f;

	ThirdPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ThirdPersonCamera"));
	ThirdPersonCamera->SetupAttachment(ThirdPersonSpringArm, USpringArmComponent::SocketName);

	FirstPersonSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("FirstPersonPivot"));
	FirstPersonSpringArm->bDoCollisionTest = false;
	FirstPersonSpringArm->SetupAttachment(Airframe, USpringArmComponent::SocketName);
	FirstPersonSpringArm->bEnableCameraLag = false;
	FirstPersonSpringArm->bEnableCameraRotationLag = true;
	FirstPersonSpringArm->CameraRotationLagSpeed = 3.f;

	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCamera->SetupAttachment(FirstPersonSpringArm);

	ManagerComp = CreateDefaultSubobject<UCameraManagerComponent>(TEXT("CameraManagerComponent"));
	AudioComp = CreateDefaultSubobject<UAircraftAudioComponent>(TEXT("AudioComponent"));

	PersonalAircraftAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("PersonalAircraftAudio"));
	PersonalAircraftAudio->SetupAttachment(UnitRoot);
	PersonalAircraftAudio->bAutoActivate = false;
	PersonalAircraftAudio->bIsUISound = false;

	GunAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("GunAudio"));
	GunAudio->SetupAttachment(UnitRoot);
	GunAudio->bAutoActivate = false;
	GunAudio->bIsUISound = false;

	health = 100;
	Faction = EFaction::Ally;
}

void APlayerAircraft::BeginPlay()
{
	Super::BeginPlay();
	AudioComp->SetControlled(this);
	ManagerComp->SetControlled(this);
	ManagerComp->SetAudioComp(AudioComp);
	OriginalFirstPersonSpringArmLength = FirstPersonSpringArm->TargetArmLength;
	OriginalThirdPersonSpringArmLength = ThirdPersonSpringArm->TargetArmLength;
	SetSensitivity(0.f);
	SetInterp();
}

void APlayerAircraft::Tick(float DeltaSeconds) 
{
	Super::Tick(DeltaSeconds);
}

void APlayerAircraft::PossessedBy(AController* NewController) 
{
	Super::PossessedBy(NewController);

	Controlled = Cast<AAircraftPlayerController>(NewController);
	if (!IsValid(Controlled)) return;
	Controlled->SetComponents(WeaponComponent);
}

void APlayerAircraft::HandleHit() 
{

}

void APlayerAircraft::WeaponComponentOnUnitDeath() 
{
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
	if (!IsValid(WeaponComponent) || WeaponComponent->WeaponGroups.Num() == 0) return 0;

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
	if (IsValid(AudioComp)) AudioComp->HandleGunSound(bShooting);
}

void APlayerAircraft::CycleTarget() { if (RadarComponent) RadarComponent->CycleTarget(); };

void APlayerAircraft::SetHUD(APlayerHUD* InHUD) 
{
	if (!IsValid(ManagerComp)) return;
	ManagerComp->SetHUD(InHUD);
	ManagerComp->SetThirdPerson();
}

float APlayerAircraft::GetRoll() { if (FlightComponent) return FlightComponent->GetRoll(); return 0; }

void APlayerAircraft::SwitchCameras() { if (IsValid(ManagerComp)) ManagerComp->SwitchCamera(); }

void APlayerAircraft::HandleVertical(float Vertical) { if (IsValid(ManagerComp)) ManagerComp->SetVertical(Vertical); }

void APlayerAircraft::HandleHorizontal(float Horizontal) { if (IsValid(ManagerComp)) ManagerComp->SetHorizontal(Horizontal); }

void APlayerAircraft::SetFirstPersonCamera(bool bActive) { if (IsValid(FirstPersonCamera)) FirstPersonCamera->SetActive(bActive); };

void APlayerAircraft::SetThirdPersonCamera(bool bActive) { if (IsValid(ThirdPersonCamera)) ThirdPersonCamera->SetActive(bActive); };

void APlayerAircraft::SetSensitivity(float Sens) {
	if (IsValid(ManagerComp)) {
		ManagerComp->Sensitivity = CameraSens;
	}
}

void APlayerAircraft::SetRollStrength(float S) {
	if (IsValid(ManagerComp)) {
		ManagerComp->RollLagStrength = RollLagStrength;
	}
}

void APlayerAircraft::SetRollSpeed(float S) {
	if (IsValid(ManagerComp)) {
		ManagerComp->RollLagSpeed = RollLagSpeed;
	}
}

void APlayerAircraft::SetInterp() {
	if (IsValid(ManagerComp)) {
		ManagerComp->SetInterp(Interp);
	}
}

float APlayerAircraft::GetThrottle() {
	if (IsValid(FlightComponent)) return FlightComponent->GetThrottle();
	return 0.f;
}