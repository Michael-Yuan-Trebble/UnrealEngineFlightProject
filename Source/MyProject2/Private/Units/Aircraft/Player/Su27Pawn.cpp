// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Su27"));
#include "Units/Aircraft/Player/Su27Pawn.h"
#include "Units/Aircraft/FlightComponent.h"
#include "Player Info/AircraftPlayerController.h"

ASu27Pawn::ASu27Pawn()
{
	PrimaryActorTick.bCanEverTick = true;

	TakeoffSpeed = 30.f;
	springArmLength = 1800.f;
}

void ASu27Pawn::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	Controlled = Cast<AAircraftPlayerController>(NewController);
}

void ASu27Pawn::Tick(float DeltaSeconds) 
{
	Super::Tick(DeltaSeconds);
	if (!Controlled || !FlightComponent) return;
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

void ASu27Pawn::PitchCalculation(float Delta)
{
	if (InputPitchValue == 0) 
	{
		Slat.Roll = FMath::FInterpTo(Slat.Roll, 0, Delta, 4);
		Slat.Pitch = FMath::FInterpTo(Slat.Pitch, 0, Delta, 4);
		Elevator = FMath::FInterpTo(Elevator, 0, Delta, 2);
		return;
	}
	if (InputPitchValue > 0) 
	{
		Slat.Roll = FMath::Clamp(InputPitchValue+ Slat.Roll, 0, 9);
		Slat.Roll = FMath::FInterpTo(Slat.Roll, InputPitchValue * 9, Delta, 5);

		Slat.Pitch = FMath::Clamp(Slat.Pitch- InputPitchValue, -10, 0);
		Slat.Pitch = FMath::FInterpTo(Slat.Pitch, InputPitchValue * -10, Delta, 5);
	}
	else 
	{
		Slat.Roll = FMath::FInterpTo(Slat.Roll, 0, Delta, 4);
		Slat.Pitch = FMath::FInterpTo(Slat.Pitch, 0, Delta, 4);
	}
	Elevator = FMath::Clamp(InputPitchValue + Elevator, -10, 10);
	Elevator = FMath::FInterpTo(Elevator, InputPitchValue * 10, Delta, 0.5);
}

void ASu27Pawn::RollCalculation(float Delta) 
{
	if (InputRollValue == 0) 
	{
		Flap.Roll = FMath::FInterpTo(Flap.Roll, 0, Delta, 2);
		Flap.Pitch = FMath::FInterpTo(Flap.Pitch, 0, Delta, 2);
		Flap.Yaw = FMath::FInterpTo(Flap.Yaw, 0, Delta, 2);
		return;
	}
	Flap.Roll = FMath::Clamp(Flap.Roll + InputRollValue, -9, 9);
	Flap.Pitch = FMath::Clamp(Flap.Pitch + InputRollValue, -20, 20);
	Flap.Yaw = FMath::Clamp(Flap.Yaw + FMath::Abs(InputRollValue), 0, 3);

	Flap.Roll = FMath::FInterpTo(Flap.Roll, InputRollValue * 9, Delta, 9/10.f);
	Flap.Pitch = FMath::FInterpTo(Flap.Pitch, InputRollValue * 20, Delta, 20/10.f);
	Flap.Yaw = FMath::FInterpTo(Flap.Yaw, InputRollValue * 3, Delta, 3/10.f);
}

void ASu27Pawn::YawCalculation(float Delta) 
{
	if (InputYawValue == 0)
	{
		Rudder.Roll = FMath::FInterpTo(Rudder.Roll, 0, Delta, 2);
		Rudder.Yaw = FMath::FInterpTo(Rudder.Yaw, 0, Delta, 2);
		return;
	}
	Rudder.Roll = FMath::Clamp(Rudder.Roll + InputYawValue, -5, 5);
	Rudder.Yaw = FMath::Clamp(Rudder.Yaw + InputYawValue, -20, 20);

	Rudder.Roll = FMath::FInterpTo(Rudder.Roll, InputYawValue * 5, Delta, 1);
	Rudder.Yaw = FMath::FInterpTo(Rudder.Yaw, InputYawValue * 20, Delta, 6);
}

void ASu27Pawn::AirBrakeCalculation(float Delta) 
{
	if (InputThrust > 0.4) 
	{
		AirBrake = FMath::FInterpTo(AirBrake, 0, Delta, 5);
		return;
	}
	AirBrake = FMath::FInterpTo(AirBrake, -40, Delta, 5);
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
		Nozzle = FMath::FInterpTo(Nozzle, 1 - reverse, Delta, 1);
		Nozzle = FMath::Clamp(Nozzle, 0.6, 1);
		return;
	}
	float Ratio = (InputThrust - 0.8) / 0.2;
	Nozzle = FMath::FInterpTo(Nozzle, 1.2 * Ratio, Delta, 1);
}