// Fill out your copyright notice in the Description page of Project Settings.

#include "Units/Aircraft/Player/PlayerAircraft.h"
#include "Player Info/AircraftPlayerController.h"
#include "Units/Components/Aircraft/FlightComponent.h"
#include "Units/Components/Aircraft/RadarComponent.h"
#include "Components/SceneComponent.h"
#include "Units/Components/Standard/HealthComponent.h"
#include "Units/Components/Aircraft/AircraftAudioComponent.h"
#include "Units/Components/Aircraft/WeaponSystemComponent.h"
#include "Components/AudioComponent.h"
#include "Units/Components/Player/CameraManagerComponent.h"
#include "Components/BoxComponent.h"
#include "Units/Components/Player/UIAudioComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

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
	UIAudioComponent = CreateDefaultSubobject<UUIAudioComponent>(TEXT("UIAudioComponent"));

	health = 100;
	Faction = EFaction::Ally;
}

void APlayerAircraft::BeginPlay()
{
	Super::BeginPlay();
	if (IsValid(AudioComp)) AudioComp->SetControlled(this);
	if (IsValid(ManagerComp)) {
		ManagerComp->SetControlled(this);
		ManagerComp->SetAudioComp(AudioComp);
	}
	if (IsValid(FirstPersonSpringArm) && IsValid(ThirdPersonSpringArm)) {
		OriginalFirstPersonSpringArmLength = FirstPersonSpringArm->TargetArmLength;
		OriginalThirdPersonSpringArmLength = ThirdPersonSpringArm->TargetArmLength;
	}

	if (IsValid(WeaponComponent)) {
		WeaponComponent->OnLockingSound.AddDynamic(this, &APlayerAircraft::HandleLockSound);
	}

	SetSensitivity(CameraSens);
	SetInterp();

	UAircraftStats* LoadedStats = AirStats.LoadSynchronous();
	if (!IsValid(LoadedStats)) return;

	if (UAircraftAudioData* LoadedAudio = LoadedStats->AudioData.LoadSynchronous())
		if (IsValid(UIAudioComponent)) UIAudioComponent->SetAudio(LoadedAudio);
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
	if (IsValid(WeaponComponent)) WeaponComponent->ResetLockedOn();
}

void APlayerAircraft::HandleLockSound(float LockPercent) {
	if (IsValid(UIAudioComponent)) {
		if (LockPercent >= 1) {
			UIAudioComponent->LockedSound(true);
			return;
		}
		UIAudioComponent->LockingSound(LockPercent > 0);
		UIAudioComponent->LockedSound(false);
	}
}

void APlayerAircraft::FireBullets() { if (IsValid(WeaponComponent)) WeaponComponent->FireBullets(); }

void APlayerAircraft::StartBullets() 
{
	GunSoundEffect(true);
	FireBullets();
	if (IsValid(CachedBulletStats)) GetWorld()->GetTimerManager().SetTimer(RepeatTimerHandle, this, &APlayerAircraft::FireBullets, CachedBulletStats->FireRate, true);
}

void APlayerAircraft::EndBullets()
{
	GunSoundEffect(false);
	GetWorld()->GetTimerManager().ClearTimer(RepeatTimerHandle);
}

void APlayerAircraft::SelectWeapon(float index) { if (IsValid(WeaponComponent)) WeaponComponent->SelectWeapon(index); }

int32 APlayerAircraft::AdvanceWeapon(int32 index, bool bForward) 
{
	if (!IsValid(WeaponComponent)) return 0;
	TArray<TSubclassOf<ABaseWeapon>> Keys;
	WeaponComponent->GetWeaponGroups().GetKeys(Keys);

	if (Keys.Num() == 0) return 0;

	if (bForward)
		index = (index + 1) % Keys.Num();
	else
		index = (index - 1 + Keys.Num()) % Keys.Num();

	SelectWeapon(index);
	return index;
}

void APlayerAircraft::GunSoundEffect(bool bShooting) { if (IsValid(AudioComp)) AudioComp->HandleGunSound(bShooting); }

void APlayerAircraft::CycleTarget() { if (IsValid(RadarComponent)) RadarComponent->CycleTarget(); };

void APlayerAircraft::SetHUD(APlayerHUD* InHUD) 
{
	if (!IsValid(ManagerComp)) return;
	ManagerComp->SetHUD(InHUD);
	ManagerComp->SetThirdPerson();
}

const FRotator& APlayerAircraft::GetNextRotation() { if (IsValid(FlightComponent)) return FlightComponent->GetNextRotation(); return FRotator::ZeroRotator; }

void APlayerAircraft::SwitchCameras() { if (IsValid(ManagerComp)) ManagerComp->SwitchCamera(); }

void APlayerAircraft::HandleVertical(float Vertical) { if (IsValid(ManagerComp)) ManagerComp->SetVertical(Vertical); }

void APlayerAircraft::HandleHorizontal(float Horizontal) { if (IsValid(ManagerComp)) ManagerComp->SetHorizontal(Horizontal); }

void APlayerAircraft::SetFirstPersonCamera(bool bActive) { if (IsValid(FirstPersonCamera)) FirstPersonCamera->SetActive(bActive); };

void APlayerAircraft::SetThirdPersonCamera(bool bActive) { if (IsValid(ThirdPersonCamera)) ThirdPersonCamera->SetActive(bActive); };

void APlayerAircraft::SetSensitivity(float Sens) { if (IsValid(ManagerComp)) ManagerComp->Sensitivity = CameraSens; }

void APlayerAircraft::SetRollStrength(float S) { if (IsValid(ManagerComp)) ManagerComp->RollLagStrength = RollLagStrength; }

void APlayerAircraft::SetRollSpeed(float S) { if (IsValid(ManagerComp)) ManagerComp->RollLagSpeed = RollLagSpeed; }

void APlayerAircraft::SetInterp() { if (IsValid(ManagerComp)) ManagerComp->SetInterp(Interp); }

float APlayerAircraft::GetThrottle() {
	if (IsValid(FlightComponent)) return FlightComponent->GetThrottle();
	return 0.f;
}