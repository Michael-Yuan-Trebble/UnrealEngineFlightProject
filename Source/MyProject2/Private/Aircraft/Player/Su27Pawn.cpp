// Fill out your copyright notice in the Description page of Project Settings.


#include "Aircraft/Player/Su27Pawn.h"
#include "Aircraft/FlightComponent.h"
#include "AircraftPlayerController.h"

ASu27Pawn::ASu27Pawn()
{
	PrimaryActorTick.bCanEverTick = true;

	TakeoffSpeed = 30.f;
	springArmLength = 1800.f;
	NumPylons = 4;
}

void ASu27Pawn::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	Controlled = Cast<AAircraftPlayerController>(NewController);
	AddPylons();
}

void ASu27Pawn::Tick(float DeltaSeconds) 
{
	Super::Tick(DeltaSeconds);
	if (!Controlled) return;
	InputPitchValue = FlightComponent->UserPitch;
	InputThrust = FlightComponent->CurrentThrust;
	InputYawValue = FlightComponent->UserYaw;
	InputRollValue = FlightComponent->UserRoll;
	PitchCalculation(DeltaSeconds);
	RollCalculation(DeltaSeconds);
	YawCalculation(DeltaSeconds);
	ThrustCalculation(DeltaSeconds);
	AirBrakeCalculation(DeltaSeconds);
}

// Slats limits: R: -9, P: -10, 0, for L
// Elevator limits: P: 10
// Nozzle limits: 0.8 (Can go to like 0.5 if really small) - 1.2 or 1.3 Scale
// Flap limits: R: 14, P: 30, Y: 4, for L
// Rudder limits: R: 5, Y: 20, for heading R
// Airbrake limits: P: -40


void ASu27Pawn::PitchCalculation(float Delta)
{
	if (InputPitchValue == 0) 
	{
		LeftSlat.Roll = FMath::FInterpTo(LeftSlat.Roll, 0, Delta, 4);
		LeftSlat.Pitch = FMath::FInterpTo(LeftSlat.Pitch, 0, Delta, 4);
		LeftElevator = FMath::FInterpTo(LeftElevator, 0, Delta, 2);
		RightSlat = LeftSlat;
		RightElevator = LeftElevator;
		return;
	}
	else if (InputPitchValue > 0) 
	{
		LeftSlat.Roll = FMath::Clamp(InputPitchValue+LeftSlat.Roll, 0, 9);
		LeftSlat.Roll = FMath::FInterpTo(LeftSlat.Roll, InputPitchValue * 9, Delta, 5);

		LeftSlat.Pitch = FMath::Clamp(LeftSlat.Pitch- InputPitchValue, -10, 0);
		LeftSlat.Pitch = FMath::FInterpTo(LeftSlat.Pitch, InputPitchValue * -10, Delta, 5);
		RightSlat = LeftSlat;
	}
	else {
		LeftSlat.Roll = FMath::FInterpTo(LeftSlat.Roll, 0, Delta, 4);
		LeftSlat.Pitch = FMath::FInterpTo(LeftSlat.Pitch, 0, Delta, 4);
		RightSlat = LeftSlat;
	}
	LeftElevator = FMath::Clamp(InputPitchValue + LeftElevator, -10, 10);
	LeftElevator = FMath::FInterpTo(LeftElevator, InputPitchValue * 10, Delta, 0.2);
	RightElevator = LeftElevator;
}

void ASu27Pawn::RollCalculation(float Delta) 
{
	if (InputRollValue == 0) 
	{
		LeftFlap.Roll = FMath::FInterpTo(LeftFlap.Roll, 0, Delta, 2);
		LeftFlap.Pitch = FMath::FInterpTo(LeftFlap.Pitch, 0, Delta, 2);
		LeftFlap.Yaw = FMath::FInterpTo(LeftFlap.Yaw, 0, Delta, 2);
		RightFlap = LeftFlap;
	}
	else 
	{
		LeftFlap.Roll = FMath::Clamp(LeftFlap.Roll + InputRollValue, -9, 9);
		LeftFlap.Pitch = FMath::Clamp(LeftFlap.Pitch + InputRollValue, -20, 20);
		LeftFlap.Yaw = FMath::Clamp(LeftFlap.Yaw + InputRollValue, -3, 3);

		LeftFlap.Roll = FMath::FInterpTo(LeftFlap.Roll, InputRollValue * 14, Delta, 9 / 20);
		LeftFlap.Pitch = FMath::FInterpTo(LeftFlap.Pitch, InputRollValue * 30, Delta, 20 / 20);
		LeftFlap.Yaw = FMath::FInterpTo(LeftFlap.Yaw, InputRollValue * 3, Delta, 3/20);
		RightFlap = LeftFlap;
	}
}

void ASu27Pawn::YawCalculation(float Delta) 
{
	if (InputYawValue == 0)
	{
		LeftRudder.Roll = FMath::FInterpTo(LeftRudder.Roll, 0, Delta, 2);
		LeftRudder.Yaw = FMath::FInterpTo(LeftRudder.Yaw, 0, Delta, 2);
		RightRudder = LeftRudder;
	}
	else 
	{
		LeftRudder.Roll = FMath::Clamp(LeftRudder.Roll + InputYawValue, -5, 5);
		LeftRudder.Yaw = FMath::Clamp(LeftRudder.Yaw + InputYawValue, -20, 20);

		LeftRudder.Roll = FMath::FInterpTo(LeftRudder.Roll, InputYawValue * 5, Delta, 6);
		LeftRudder.Yaw = FMath::FInterpTo(LeftRudder.Yaw, InputYawValue * 20, Delta, 2);
		RightRudder = LeftRudder;
	}
}

void ASu27Pawn::AirBrakeCalculation(float Delta) 
{
	if (InputThrust == 0) 
	{
		AirBrake = FMath::FInterpTo(AirBrake, 0, Delta, 10);
	}
	else 
	{
		AirBrake = FMath::FInterpTo(AirBrake, -40, Delta, 10);
	}
}
/*
	Non Afterburner = 0-80
	Afterburner = 81 - 100
*/

void ASu27Pawn::ThrustCalculation(float Delta) 
{
	if (InputThrust >= 0 && InputThrust <= 0.8) 
	{
		float reverse = InputThrust / 2;
		LeftNozzle = FMath::FInterpTo(LeftNozzle, 1 - reverse, Delta, 1);
		LeftNozzle = FMath::Clamp(LeftNozzle, 0.6, 1);
	}
	else
	{
		float Ratio = (InputThrust - 0.8) / 0.2;
		LeftNozzle = FMath::FInterpTo(LeftNozzle, 1.2 * Ratio, Delta, 1);
	}
	RightNozzle = LeftNozzle;
}