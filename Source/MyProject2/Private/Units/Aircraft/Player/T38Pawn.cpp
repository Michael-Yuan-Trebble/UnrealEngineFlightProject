// Fill out your copyright notice in the Description page of Project Settings.


#include "Units/Aircraft/Player/T38Pawn.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Units/Aircraft/FlightComponent.h"
#include "Animation/AnimInstance.h"
#include "Player Info/AircraftPlayerController.h"

AT38Pawn::AT38Pawn() 
{
	PrimaryActorTick.bCanEverTick = true;

	TakeoffSpeed = 30.f;
	springArmLength = 1800.f;
}

void AT38Pawn::PossessedBy(AController* NewController) 
{
	Super::PossessedBy(NewController);
	Controlled = Cast<AAircraftPlayerController>(NewController);
}

void AT38Pawn::Tick(float DeltaSeconds) 
{
	Super::Tick(DeltaSeconds);
	if (!Controlled) return;
	InputPitchValue = FlightComponent->UserPitch;
	InputThrust = FlightComponent->CurrentThrust;
	InputYawValue = FlightComponent->UserYaw;
	InputRollValue = FlightComponent->UserRoll;
	RollCalculation(DeltaSeconds);
	PitchCalculation(DeltaSeconds);
}

void AT38Pawn::PitchCalculation(float DeltaSeconds) 
{
	if (InputPitchValue == 0)
	{
		LeftElevator = FMath::FInterpTo(LeftElevator, 0, DeltaSeconds, 4);
		RightElevator = FMath::FInterpTo(RightElevator, 0, DeltaSeconds, 4);
		return;
	}
	LeftElevator = FMath::Clamp((InputPitchValue)+LeftElevator, -20.f, 20.f);
	LeftElevator = FMath::FInterpTo(LeftElevator, InputPitchValue * 20.f, DeltaSeconds, 0.2);
	RightElevator = LeftElevator;
}

void AT38Pawn::RollCalculation(float DeltaSeconds) 
{
	if (InputRollValue == 0)
	{
		LeftTurningFlap = FMath::FInterpTo(LeftTurningFlap, 0, DeltaSeconds, 2);
		RightTurningFlap = FMath::FInterpTo(RightTurningFlap, 0, DeltaSeconds, 2);
		return;
	}
	LeftTurningFlap = FMath::Clamp(LeftTurningFlap + InputRollValue, -40.f, 40.f);
	LeftTurningFlap = FMath::FInterpTo(LeftTurningFlap, InputRollValue * 40.f, DeltaSeconds, 0.2);
	RightTurningFlap = LeftTurningFlap;
}

void AT38Pawn::YawCalculation(float DeltaSeconds)
{
	if (InputYawValue == 0)
	{
		Rudder = FMath::FInterpTo(Rudder, 0, DeltaSeconds, 2);
		return;
	}
	Rudder = FMath::Clamp(InputYawValue + Rudder, -20, 20);
	Rudder = FMath::FInterpTo(Rudder, InputYawValue * 20, DeltaSeconds, 0.2);
}

void AT38Pawn::AirBrakeCalculation(float DeltaSeconds) 
{
	if (InputThrust <= -0.9) 
	{
		LeftAirBrake = FMath::FInterpTo(LeftAirBrake, -90, DeltaSeconds, 2.f);
		RightAirBrake = LeftAirBrake;
		return;
	}
	LeftAirBrake = FMath::FInterpTo(LeftAirBrake, 0, DeltaSeconds, 2.f);
	RightAirBrake = LeftAirBrake;
}