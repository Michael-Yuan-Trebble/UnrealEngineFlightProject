// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Camera Manager!"));
#include "Aircraft/Player/CameraManagerComponent.h"
#include "Aircraft/Player/PlayerAircraft.h"
#include "Aircraft/AircraftAudioComponent.h"
#include "UI/PlayerHUD.h"
#include "Kismet/GameplayStatics.h"

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
	if (!PC || !Controlled) return;

	CurrentMode = ECameraPerspective::FirstPerson;

	LookXLock = 120.f;
	LookYLock = 60.f;
	FirstPersonX = 0.f;
	FirstPersonY = 0.f;

	Controlled->FirstPersonSpringArm->SetRelativeRotation(FRotator::ZeroRotator);

	PC->SetViewTargetWithBlend(Controlled, 0.f, VTBlend_Cubic);
	Controlled->FirstPersonCamera->SetActive(true);
	Controlled->ThirdPersonCamera->SetActive(false);

	if (HUD) HUD->TogglePitchLadder(false);
	if (AudioComp) AudioComp->PlayPerspectiveSound(CurrentMode);
}

void UCameraManagerComponent::SetThirdPerson() 
{
	APlayerController* PC = Cast<APlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	if (!PC || !Controlled) return;

	CurrentMode = ECameraPerspective::ThirdPerson;

	LookXLock = 180.f;
	LookYLock = 85.f;
	currentX = 0.f;
	currentY = 0.f;

	if (SpringArm) SpringArm->SetRelativeRotation(FRotator::ZeroRotator);

	Controlled->FirstPersonCamera->SetActive(false);
	Controlled->ThirdPersonCamera->SetActive(true);

	PC->SetViewTargetWithBlend(Controlled, 0.f, VTBlend_Cubic);

	if (HUD) HUD->TogglePitchLadder(true);
	if (AudioComp) AudioComp->PlayPerspectiveSound(CurrentMode);
}

void UCameraManagerComponent::LookHor(float lookX) 
{
	switch (CurrentMode)
	{
	case ECameraPerspective::FirstPerson:
		FirstPersonHorizontal(lookX);
		break;
	case ECameraPerspective::ThirdPerson:
		ThirdPersonHorizontal(lookX);
		break;
	default:
		ThirdPersonHorizontal(lookX);
		break;
	}
}

void UCameraManagerComponent::LookVer(float lookY) 
{
	switch (CurrentMode) 
	{
		case ECameraPerspective::FirstPerson:
			FirstPersonVertical(lookY);
			break;
		case ECameraPerspective::ThirdPerson:
			ThirdPersonVertical(lookY);
			break;
		default:
			ThirdPersonVertical(lookY);
			break;
	}
}

void UCameraManagerComponent::FirstPersonHorizontal(float X) 
{
	if (!Controlled || !Controlled->FirstPersonSpringArm) return;

	FirstPersonPrevX = FirstPersonX;

	FirstPersonX += X;

	FirstPersonX = FMath::Clamp(FirstPersonX, -LookXLock, LookXLock);

	FRotator NewRot = Controlled->FirstPersonSpringArm->GetRelativeRotation();
	NewRot.Yaw = FirstPersonX;
	Controlled->FirstPersonSpringArm->SetRelativeRotation(NewRot);
}

void UCameraManagerComponent::FirstPersonVertical(float Y) 
{
	if (!Controlled || !Controlled->FirstPersonSpringArm) return;

	Y = FMath::Abs(Y) == 0 ? 0 : Y;

	FirstPersonPrevY = FirstPersonY;
	FirstPersonY += Y;

	FirstPersonY = FMath::Clamp(FirstPersonY, -LookYLock, LookYLock);

	if (FirstPersonY == LookYLock)
	{
		Y = LookYLock - FirstPersonPrevY;
	}
	else if (FirstPersonY == -LookYLock)
	{
		Y = LookYLock + FirstPersonPrevY;
	}
	Controlled->FirstPersonSpringArm->AddRelativeRotation(FRotator(Y, 0.f, 0.f));
}

void UCameraManagerComponent::ThirdPersonHorizontal(float X) 
{
	if (!SpringArm) return;
	X = FMath::Abs(X) == 0 ? 0 : X;

	prevX = currentX;
	currentX += X;

	currentX = FMath::Clamp(currentX, -LookXLock, LookXLock);

	if (currentX == -LookXLock || currentX == LookXLock) X = LookXLock - prevX;

	SpringArm->AddRelativeRotation(FRotator(0.f, X, 0.f));
}

void UCameraManagerComponent::ThirdPersonVertical(float Y) 
{
	if (!SpringArm) return;
	Y = FMath::Abs(Y) == 0 ? 0 : Y;

	prevY = currentY;
	currentY += Y;

	currentY = FMath::Clamp(currentY, -LookYLock, LookYLock);

	if (currentY == LookYLock)
	{
		Y = LookYLock - prevY;
	}
	else if (currentY == -LookYLock)
	{
		Y = LookYLock + prevY;
	}
	SpringArm->AddRelativeRotation(FRotator(Y, 0.f, 0.f));
}