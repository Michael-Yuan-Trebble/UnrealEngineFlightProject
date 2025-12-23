// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Visual Comp!"));
#include "Aircraft/AircraftVisualComponent.h"

#define INTERPSPEED 30

UAircraftVisualComponent::UAircraftVisualComponent() { PrimaryComponentTick.bCanEverTick = true; }

void UAircraftVisualComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) 
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	PitchCalculation(DeltaTime);
	RollCalculation(DeltaTime);
	YawCalculation(DeltaTime);
	ThrustCalculation(DeltaTime);
	AirBrakeCalculation(DeltaTime);

	// In order to account for both pitch and roll playing a roll
	Elevator = ElevatorPitch + ElevatorRoll;
	Flap = FlapPitch + FlapRoll;
}

#define MAXSLATS AircraftInfo.MaxSlats
#define MAXPITCHELEVATOR AircraftInfo.MaxPitchElevator
#define MAXPITCHFLAPS AircraftInfo.MaxPitchFlaps
#define MINPITCHFLAPS AircraftInfo.MinPitchFlaps

void UAircraftVisualComponent::PitchCalculation(float D) 
{
	// Leading Edge Slats

	if (bSlats) 
	{
		if (InputPitch <= 0) Slat = FMath::FInterpTo(Slat, 0, D, 4);
		else Slat = FMath::FInterpTo(Slat, FMath::Clamp(Slat + InputPitch, 0, MAXSLATS), D, INTERPSPEED);
	}

	// Elevators pitch slightly
	// TODO: When delimiting AOA, elevators' max should be increased

	if (bElevators) 
	{
		if (InputPitch == 0) ElevatorPitch = FMath::FInterpTo(ElevatorPitch, 0, D, INTERPSPEED);
		else ElevatorPitch = FMath::FInterpTo(ElevatorPitch, FMath::Clamp(ElevatorPitch + InputPitch, -MAXPITCHELEVATOR, MAXPITCHELEVATOR), D, INTERPSPEED);
	}

	// TODO: Currently Flaps are inverses of each other, somehow change that

	if (InputPitch <= 0) FlapPitch = FMath::FInterpTo(FlapPitch, 0, D, INTERPSPEED);
	else FlapPitch = FMath::FInterpTo(FlapPitch, FMath::Clamp(FlapPitch + InputPitch, MINPITCHFLAPS, MAXPITCHFLAPS), D, INTERPSPEED);

	if (bCanards) 
	{
		// TODO: Once there are canard aircraft add something for this
	}
}

#define MAXRUDDER AircraftInfo.MaxRudder

void UAircraftVisualComponent::YawCalculation(float D) 
{
	// Rudder
	if (InputYaw == 0) Rudder = FMath::FInterpTo(Rudder, 0, D, INTERPSPEED);
	else Rudder = FMath::FInterpTo(Rudder, FMath::Clamp(Rudder + InputYaw, -MAXRUDDER, MAXRUDDER), D, INTERPSPEED);
}

#define MAXROLLFLAPS AircraftInfo.MaxRollFlaps
#define MAXELEVATORROLL AircraftInfo.MaxRollElevator

void UAircraftVisualComponent::RollCalculation(float D) 
{
	// Flaps roll equally
	if (InputRoll == 0) FlapRoll = FMath::FInterpTo(FlapRoll, 0, D, INTERPSPEED);
	else FlapRoll = FMath::FInterpTo(FlapRoll, FMath::Clamp(FlapRoll + InputRoll, -MAXROLLFLAPS, MAXROLLFLAPS), D, INTERPSPEED);

	// Elevators have a slight pitch when rolling

	if (bElevators)
	{
		if (InputRoll == 0) ElevatorRoll = FMath::FInterpTo(ElevatorRoll, 0, D, INTERPSPEED);
		else ElevatorRoll = FMath::FInterpTo(ElevatorRoll, FMath::Clamp(ElevatorRoll + InputRoll, -MAXELEVATORROLL, MAXELEVATORROLL), D, INTERPSPEED);
	}
}

#define AFTERBURNERPERCENT 0.8
#define DIFFERENCE 1 - AFTERBURNERPERCENT 
#define MINNOZZLE AircraftInfo.MinExhaust
#define MAXNOZZLE AircraftInfo.MaxExhaust

void UAircraftVisualComponent::ThrustCalculation(float D) 
{
	// All Nozzles contract and expand equally

	if (bNozzle) 
	{
		if (InputThrust <= AFTERBURNERPERCENT)
		{
			float reverse = InputThrust / 2;
			Nozzle = FMath::FInterpTo(Nozzle, FMath::Clamp(1 - reverse, MINNOZZLE, MAXNOZZLE), D, INTERPSPEED);
		}
		else 
		{
			float Ratio = (InputThrust - AFTERBURNERPERCENT) / DIFFERENCE;
			Nozzle = FMath::FInterpTo(Nozzle, FMath::Clamp(MAXNOZZLE * Ratio, MINNOZZLE, MAXNOZZLE), D, INTERPSPEED);
		}
	}
}

#define SLOWSPEED 0.4
#define MAXAIRBRAKE AircraftInfo.MaxAirbrake

void UAircraftVisualComponent::AirBrakeCalculation(float D)
{
	// Might move this also into thrust, depends if it doesn't make too much sense when expanded

	if (InputThrust > SLOWSPEED) AirBrake = FMath::FInterpTo(AirBrake, 0, D, INTERPSPEED);
	else AirBrake = FMath::FInterpTo(AirBrake, MAXAIRBRAKE, D, INTERPSPEED);
}