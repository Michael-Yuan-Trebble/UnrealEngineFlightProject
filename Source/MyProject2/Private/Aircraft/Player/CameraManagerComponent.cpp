// Fill out your copyright notice in the Description page of Project Settings.

#include "Aircraft/Player/CameraManagerComponent.h"
#include "Aircraft/Player/PlayerAircraft.h"
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
	if (Next > (int)ECameraMode::ThirdPerson) Next = 0;
	CurrentMode = (ECameraMode)Next;
	switch (CurrentMode) 
	{
		case ECameraMode::FirstPerson:
			SetFirstPerson();
			break;
		case ECameraMode::ThirdPerson:
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

	CurrentMode = ECameraMode::FirstPerson;
	Controlled->FirstPersonCamera->SetActive(true);
	Controlled->ThirdPersonCamera->SetActive(false);

	PC->SetViewTargetWithBlend(Controlled, 0.3f, VTBlend_Cubic);

	LookXLock = 120.f;
	LookYLock = 60.f;
	currentX = 0.f;
	currentY = 0.f;

	Controlled->FirstPersonCamera->SetRelativeRotation(FRotator::ZeroRotator);
}

void UCameraManagerComponent::SetThirdPerson() 
{
	APlayerController* PC = Cast<APlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	if (!PC || !Controlled) return;

	CurrentMode = ECameraMode::ThirdPerson;
	Controlled->FirstPersonCamera->SetActive(false);
	Controlled->ThirdPersonCamera->SetActive(true);

	PC->SetViewTargetWithBlend(Controlled, 0.3f, VTBlend_Cubic);

	LookXLock = 180.f;
	LookYLock = 85.f;
	currentX = 0.f;
	currentY = 0.f;

	if (!SpringArm) return;

	SpringArm->SetRelativeRotation(FRotator::ZeroRotator);
}

void UCameraManagerComponent::LookHor(float lookX) 
{
	switch (CurrentMode)
	{
	case ECameraMode::FirstPerson:
		FirstPersonHorizontal(lookX);
		break;
	case ECameraMode::ThirdPerson:
		ThirdPersonHorizontal(lookX);
		break;
	default:
		ThirdPersonHorizontal(lookX);
	}
}

void UCameraManagerComponent::LookVer(float lookY) 
{
	switch (CurrentMode) 
	{
		case ECameraMode::FirstPerson:
			FirstPersonVertical(lookY);
			break;
		case ECameraMode::ThirdPerson:
			ThirdPersonVertical(lookY);
			break;
		default:
			ThirdPersonVertical(lookY);
	}
}

void UCameraManagerComponent::FirstPersonHorizontal(float X) 
{
	if (!Controlled || !Controlled->FirstPersonCamera) return;

	FirstPersonX += X;
	FirstPersonX = FMath::Clamp(FirstPersonX, -LookXLock, LookXLock);
	FRotator NewRot = FRotator(0.f, FirstPersonX, 0.f);
	Controlled->FirstPersonCamera->SetRelativeRotation(NewRot);
}

void UCameraManagerComponent::FirstPersonVertical(float Y) {
	if (!Controlled || !Controlled->FirstPersonCamera) return;

	FirstPersonY += Y;
	FirstPersonY = FMath::Clamp(FirstPersonY, -LookYLock, LookYLock);
	FRotator NewRot = FRotator(FirstPersonY,0.f, 0.f);
	Controlled->FirstPersonCamera->SetRelativeRotation(NewRot);
}

void UCameraManagerComponent::ThirdPersonHorizontal(float X) 
{
	if (!SpringArm) return;
	X = FMath::Abs(X) == 0 ? 0 : X;

	prevX = currentX;
	currentX += X;

	// Lock the camera at 180 degrees horizontally

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

	// Lock the camera at 85 degrees vertically
	currentY = FMath::Clamp(currentY, -LookYLock, LookYLock);

	if (currentY == LookYLock)
	{
		Y = LookYLock - prevY;
	}
	if (currentY == -85)
	{
		Y = LookYLock + prevY;
	}
	SpringArm->AddRelativeRotation(FRotator(Y, 0.f, 0.f));
}