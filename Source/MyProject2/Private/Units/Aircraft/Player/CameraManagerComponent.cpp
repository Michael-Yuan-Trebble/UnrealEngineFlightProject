// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Camera Manager!"));
#include "Units/Aircraft/Player/CameraManagerComponent.h"
#include "Units/Aircraft/Player/PlayerAircraft.h"
#include "Units/Aircraft/AircraftAudioComponent.h"
#include "UI/PlayerHUD.h"
#include "Kismet/GameplayStatics.h"

#define INTERPSPEED InterpSpeed

UCameraManagerComponent::UCameraManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCameraManagerComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UCameraManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (bXZero) LookX = FMath::FInterpTo(LookX, 0, DeltaTime, INTERPSPEED);
	if (bYZero) LookY = FMath::FInterpTo(LookY, 0, DeltaTime, INTERPSPEED);
	HandleHorizontal();
	HandleVertical();
	HandleRollLag(DeltaTime);
	if (IsValid(Controlled)) SetSpringArm(Controlled->GetThrottle(), DeltaTime);
}

void UCameraManagerComponent::HandleRollLag(float D) {
	if (!IsValid(MainSpringArm) || !IsValid(Controlled)) return;

	TargetRollOffset = FMath::Clamp(Controlled->GetRoll() * RollLagStrength,-80.f,80.f);

	CameraRollOffset = FMath::FInterpTo(
		CameraRollOffset,
		TargetRollOffset,
		D,
		RollLagSpeed
	);

	FRotator Rot = MainSpringArm->GetRelativeRotation();
	Rot.Roll = CameraRollOffset;
	MainSpringArm->SetRelativeRotation(Rot);
}

void UCameraManagerComponent::SetSpringArm(float Throttle, float D) {
	if (!IsValid(MainSpringArm)) return;
	float Org = 0.f;
	float Delta = 0.f;
	if (MainSpringArm == ThirdPersonSpringArm) 
	{
		Org = ThirdPersonLength;
		Delta = 200 * (0.5 - Throttle);
	}
	else 
	{ 
		Org = FirstPersonLength; 
		Delta = 10 * (0.5 - Throttle);
	}
	MainSpringArm->TargetArmLength = FMath::FInterpTo(MainSpringArm->TargetArmLength, Org - Delta, D, 5.f);
}

void UCameraManagerComponent::SetHorizontal(float X)
{
	if (GetWorld()) LookX = FMath::FInterpTo(LookX, X * Sensitivity, GetWorld()->GetDeltaSeconds(), INTERPSPEED);
	bXZero = FMath::IsNearlyZero(X);
}

void UCameraManagerComponent::SetVertical(float Y) 
{
	if (GetWorld()) LookY = FMath::FInterpTo(LookY, Y * Sensitivity, GetWorld()->GetDeltaSeconds(), INTERPSPEED);
	bYZero = FMath::IsNearlyZero(Y);
}

void UCameraManagerComponent::SwitchCamera() 
{
	int Next = (int)CurrentMode + 1;
	if (Next > (int)ECameraPerspective::ThirdPerson) Next = 0;
	CurrentMode = (ECameraPerspective)Next;
	switch (CurrentMode) 
	{
		case ECameraPerspective::FirstPerson:
			SetFirstPerson();
			break;
		case ECameraPerspective::ThirdPerson:
			SetThirdPerson();
			break;
		default:
			SetThirdPerson();
			break;
	}
}

void UCameraManagerComponent::SetFirstPerson() 
{
	APlayerController* PC = Cast<APlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	if (!IsValid(PC) || !IsValid(Controlled)) return;

	CurrentMode = ECameraPerspective::FirstPerson;

	LookXLock = 120.f;
	LookYLock = 60.f;

	if (!IsValid(FirstPersonSpringArm)) { 
		FirstPersonSpringArm = Controlled->GetFirstPersonSpringArm(); 
		if (IsValid(FirstPersonSpringArm)) FirstPersonLength = FirstPersonSpringArm->TargetArmLength;
		else return;
	}

	FRotator Rotation = FRotator::ZeroRotator;

	if (IsValid(MainSpringArm)) 
	{
		Rotation = MainSpringArm->GetRelativeRotation();
		Rotation.Pitch = FMath::Clamp(Rotation.Pitch, -LookYLock, LookYLock);
		Rotation.Yaw = FMath::Clamp(Rotation.Yaw, -LookXLock, LookXLock);
	}

	MainSpringArm = FirstPersonSpringArm;
	MainSpringArm->SetRelativeRotation(Rotation);

	Controlled->SetFirstPersonCamera(true);
	Controlled->SetThirdPersonCamera(false);

	PC->SetViewTargetWithBlend(Controlled, 0.f, VTBlend_Cubic);

	if (IsValid(HUD)) HUD->TogglePitchLadder(false);
	if (IsValid(AudioComp)) AudioComp->PlayPerspectiveSound(CurrentMode);
}

void UCameraManagerComponent::SetThirdPerson() 
{
	APlayerController* PC = Cast<APlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	if (!IsValid(PC) || !IsValid(Controlled)) return;

	CurrentMode = ECameraPerspective::ThirdPerson;

	LookXLock = 180.f;
	LookYLock = 85.f;

	if (!IsValid(ThirdPersonSpringArm)) { 
		ThirdPersonSpringArm = Controlled->GetThirdPersonSpringArm(); 
		if (IsValid(ThirdPersonSpringArm)) ThirdPersonLength = ThirdPersonSpringArm->TargetArmLength;
		else return;
	}

	FRotator Rotation = FRotator::ZeroRotator;

	if (IsValid(MainSpringArm)) 
	{
		Rotation = MainSpringArm->GetRelativeRotation();

		Rotation.Pitch = FMath::Clamp(Rotation.Pitch, -LookYLock, LookYLock);
		Rotation.Yaw = FMath::Clamp(Rotation.Yaw, -LookXLock, LookXLock);
	}

	MainSpringArm = ThirdPersonSpringArm;
	MainSpringArm->SetRelativeRotation(Rotation);

	Controlled->SetFirstPersonCamera(false);
	Controlled->SetThirdPersonCamera(true);

	PC->SetViewTargetWithBlend(Controlled, 0.f, VTBlend_Cubic);

	if (IsValid(HUD)) HUD->TogglePitchLadder(true);
	if (IsValid(AudioComp)) AudioComp->PlayPerspectiveSound(CurrentMode);
}

void UCameraManagerComponent::HandleHorizontal()
{
	if (!IsValid(Controlled) || !IsValid(MainSpringArm)) return;

	FRotator Rotation = MainSpringArm->GetRelativeRotation();

	Rotation.Yaw = FMath::Clamp(Rotation.Yaw + LookX, -LookXLock, LookXLock);

	MainSpringArm->SetRelativeRotation(Rotation);
}

void UCameraManagerComponent::HandleVertical() 
{
	if (!IsValid(Controlled) || !IsValid(MainSpringArm)) return;

	FRotator Rotation = MainSpringArm->GetRelativeRotation();

	Rotation.Pitch = FMath::Clamp(Rotation.Pitch + LookY, -LookYLock, LookYLock);

	MainSpringArm->SetRelativeRotation(Rotation);
}