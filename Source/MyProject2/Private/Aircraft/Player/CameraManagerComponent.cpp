// Fill out your copyright notice in the Description page of Project Settings.

#include "Aircraft/Player/CameraManagerComponent.h"

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

void UCameraManagerComponent::LookHor(float lookX) 
{
	if (!SpringArm) return;
	lookX = FMath::Abs(lookX) == 0 ? 0 : lookX;

	prevX = currentX;
	currentX += lookX;

	// Lock the camera at 180 degrees horizontally

	if (currentX >= 180)
	{
		lookX = 180 - prevX;
		currentX = 180;
	}
	if (currentX <= -180)
	{
		lookX = 180 - prevX;
		currentX = -180;
	}

	SpringArm->AddRelativeRotation(FRotator(0.f, lookX, 0.f));
}

void UCameraManagerComponent::LookVer(float lookY) 
{
	if (!SpringArm) return;
	lookY = FMath::Abs(lookY) == 0 ? 0 : lookY;

	prevY = currentY;
	currentY += lookY;

	// Lock the camera at 85 degrees vertically

	if (currentY >= 85)
	{
		lookY = 85 - prevY;
		currentY = 85;
	}
	if (currentY <= -85)
	{
		lookY = 85 + prevY;
		currentY = -85;
	}
	SpringArm->AddRelativeRotation(FRotator(lookY, 0.f, 0.f));
}
