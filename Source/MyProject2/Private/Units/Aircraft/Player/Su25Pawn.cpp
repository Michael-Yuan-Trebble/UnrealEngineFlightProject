// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("SU!"));
#include "Units/Aircraft/Player/Su25Pawn.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Animation/AnimInstance.h"
#include "Units/Aircraft/FlightComponent.h"
#include "Player Info/AircraftPlayerController.h"
#include "Weapons/Missiles/BaseIRMissile.h"
#include "Weapons/AircraftBullet.h"

bool isGearUp;
float Speed;

//Initialize Su25 Player Pawn
ASu25Pawn::ASu25Pawn() 
{
	PrimaryActorTick.bCanEverTick = true;

	TakeoffSpeed = 10.f;
	springArmLength = 400.f;
	isGearUp = false;
}
void ASu25Pawn::PossessedBy(AController* NewController) 
{
	Super::PossessedBy(NewController);

	Controlled = Cast<AAircraftPlayerController>(NewController);

	//Put Gears down if Grounded
	if (isGearUp == false)
	{
		FrontWheelCoverTop = 90;
		FrontWheelCoverBottom = 90;
		FrontWheel = 90;

		BackWheelCoverSide = -90;
		BackWheelCoverBody = 90;
		BackWheel = 90;
	}
}

void ASu25Pawn::BeginPlay()
{
	Super::BeginPlay();
}

void ASu25Pawn::Tick(float DeltaSeconds) 
{
	Super::Tick(DeltaSeconds);
	if (Controlled) 
	{
		
		//Calculate Animation Values
		InputPitchValue = FlightComponent->UserPitch;
		InputThrust = FlightComponent->CurrentThrust;
		InputYawValue = FlightComponent->UserYaw;
		InputRollValue = FlightComponent->UserRoll;
		
		if ((FlightComponent->currentSpeed > TakeoffSpeed + 5) && !isGearUp)
		{
			WheelCalculation(DeltaSeconds);
			if (FrontWheelCoverTop == 0) 
			{
				isGearUp = true;
			}
		}
		PitchElevatorCalculation(DeltaSeconds);
		AirbrakeCalculation(DeltaSeconds);
		RudderYawCalculation(DeltaSeconds);
		RollElevatorCalculation(DeltaSeconds);
		if (FlightComponent->currentSpeed < TakeoffSpeed)
		{
			MiddleFlap.Roll = FMath::FInterpTo(MiddleFlap.Roll, 0.1, DeltaSeconds, 2.f);
			MiddleFlap.Pitch = FMath::FInterpTo(MiddleFlap.Pitch, 20, DeltaSeconds, 2.f);
			InnerFlap = FMath::FInterpTo(InnerFlap, 20, DeltaSeconds, 2.f);
		}
		else 
		{
			MiddleFlap.Roll = FMath::FInterpTo(MiddleFlap.Roll, 0, DeltaSeconds, 2.f);
			MiddleFlap.Pitch = FMath::FInterpTo(MiddleFlap.Pitch, 0, DeltaSeconds, 2.f);
			InnerFlap = FMath::FInterpTo(InnerFlap, 0, DeltaSeconds, 2.f);
		}
	}
}

//Animation Functions

void ASu25Pawn::PitchElevatorCalculation(float DeltaSeconds) 
{
	if (InputPitchValue == 0) 
	{
		ElevatorMain.Pitch = FMath::FInterpTo(ElevatorMain.Pitch, 0, DeltaSeconds, 4);
		ElevatorMain.Yaw = FMath::FInterpTo(ElevatorMain.Yaw, 0, DeltaSeconds, 4);
		ElevatorMain.Roll = FMath::FInterpTo(ElevatorMain.Roll, 0, DeltaSeconds, 4);
		ElevatorSub.X = FMath::FInterpTo(ElevatorSub.X, 0, DeltaSeconds, 4);
		ElevatorSub.Z = FMath::FInterpTo(ElevatorSub.Z, 0, DeltaSeconds, 4);
		return;
	}
	ElevatorMain.Pitch = FMath::Clamp(((InputPitchValue) +ElevatorMain.Pitch), -20.f, 20.f);
	ElevatorMain.Pitch = FMath::FInterpTo(ElevatorMain.Pitch, InputPitchValue * 20.f, DeltaSeconds, 0.2);

	ElevatorMain.Yaw = FMath::Clamp(((InputPitchValue) +ElevatorMain.Yaw), -2.f, 2.f);
	ElevatorMain.Yaw = FMath::FInterpTo(ElevatorMain.Yaw, InputPitchValue * 2.f, DeltaSeconds, 0.2);

	ElevatorMain.Roll = FMath::Clamp(((InputPitchValue)+ElevatorMain.Roll), -0.7f, 0.7f);
	ElevatorMain.Roll = FMath::FInterpTo(ElevatorMain.Roll, InputPitchValue * 0.7f, DeltaSeconds, 0.2);

	ElevatorSub.X = FMath::Clamp(((InputPitchValue)+ElevatorSub.X), -5.f, 5.f);
	ElevatorSub.X = FMath::FInterpTo(ElevatorSub.X, InputPitchValue * 5.f, DeltaSeconds, 0.2);

	ElevatorSub.Z = FMath::Clamp(((InputPitchValue)+ElevatorSub.Z), -12.f, 12.f);
	ElevatorSub.Z = FMath::FInterpTo(ElevatorSub.Z, InputPitchValue * 12.f, DeltaSeconds, 0.2);
}

void ASu25Pawn::RollElevatorCalculation(float DeltaSeconds) 
{
	if (InputRollValue == 0)
	{
		OuterFlap.Roll = FMath::FInterpTo(OuterFlap.Roll, 0, DeltaSeconds, 2.f);
		OuterFlap.Pitch = FMath::FInterpTo(OuterFlap.Pitch, 0, DeltaSeconds, 2.f);
		OuterFlap.Yaw = FMath::FInterpTo(OuterFlap.Yaw, 0, DeltaSeconds, 2.f);
		return;
	}
	OuterFlap.Roll = FMath::Clamp(((InputPitchValue)+OuterFlap.Roll), -1.2, 1.2f);
	OuterFlap.Pitch = FMath::Clamp(((InputPitchValue)+OuterFlap.Pitch), -15.f, 15.f);
	OuterFlap.Yaw = FMath::Clamp(((InputPitchValue)+OuterFlap.Yaw), -0.5f, 0.5f);

	OuterFlap.Roll = FMath::FInterpTo(OuterFlap.Roll, InputRollValue * 1.2, DeltaSeconds, 2.f);
	OuterFlap.Pitch = FMath::FInterpTo(OuterFlap.Pitch, InputRollValue * 15, DeltaSeconds, 2.f);
	OuterFlap.Yaw = FMath::FInterpTo(OuterFlap.Yaw, InputRollValue * 0.5, DeltaSeconds, 2.f);
}

void ASu25Pawn::SlatsElevationCalculation(float DeltaSeconds)
{
}

void ASu25Pawn::RudderYawCalculation(float DeltaSeconds)
{
	if (InputYawValue == 0) 
	{
		RudderTopYaw = FMath::FInterpTo(RudderTopYaw, 0, DeltaSeconds, 2.f);
		RudderBottomYaw = FMath::FInterpTo(RudderBottomYaw, 0, DeltaSeconds, 2.f);
		return;
	}
	if (InputYawValue < 0)
	{
		RudderTopYaw = FMath::FInterpTo(RudderTopYaw, 35, DeltaSeconds, 2.f);
		RudderBottomYaw = FMath::FInterpTo(RudderBottomYaw, -30, DeltaSeconds, 2.f);
		return;
	}
	RudderTopYaw = FMath::FInterpTo(RudderTopYaw, -35, DeltaSeconds, 2.f);
	RudderBottomYaw = FMath::FInterpTo(RudderBottomYaw, 30, DeltaSeconds, 2.f);
}

void ASu25Pawn::AirbrakeCalculation(float DeltaSeconds) 
{
	if (InputThrust <= -0.9) 
	{
		AirbrakeTop = FMath::FInterpTo(AirbrakeTop, -80, DeltaSeconds, 2.f);
		AirbrakeBottom = FMath::FInterpTo(AirbrakeTop, 80, DeltaSeconds, 2.f);
		return;
	}
	AirbrakeTop = FMath::FInterpTo(AirbrakeTop, 0, DeltaSeconds, 2.f);
	AirbrakeBottom = FMath::FInterpTo(AirbrakeTop, 0, DeltaSeconds, 2.f);
}

void ASu25Pawn::WheelCalculation(float DeltaSeconds)
{
	FrontWheelCoverTop = FMath::FInterpTo(FrontWheelCoverTop, 0, DeltaSeconds, 2.f);
	FrontWheelCoverBottom = FMath::FInterpTo(FrontWheelCoverBottom, 0, DeltaSeconds, 2.f);
	FrontWheel = FMath::FInterpTo(FrontWheel, 0, DeltaSeconds, 2.f);

	BackWheelCoverSide = FMath::FInterpTo(BackWheelCoverSide, 0, DeltaSeconds, 2.f);
	BackWheelCoverBody = FMath::FInterpTo(BackWheelCoverBody, 0, DeltaSeconds, 2.f);
	BackWheel = FMath::FInterpTo(BackWheel, 0, DeltaSeconds, 2.f);
}