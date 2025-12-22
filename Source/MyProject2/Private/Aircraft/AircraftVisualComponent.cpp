// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Visual Comp!"));
#include "Aircraft/AircraftVisualComponent.h"

#define INTERPSPEED 30

UAircraftVisualComponent::UAircraftVisualComponent() { 
	PrimaryComponentTick.bCanEverTick = true;
}

void UAircraftVisualComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) 
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	PitchCalculation(DeltaTime);
	RollCalculation(DeltaTime);
	YawCalculation(DeltaTime);
	ThrustCalculation(DeltaTime);
	AirBrakeCalculation(DeltaTime);

	Elevator = ElevatorPitch + ElevatorRoll;
	Flap = FlapPitch + FlapRoll;
}

#define MAXSLATS AircraftInfo.MaxSlats
#define MAXPITCHELEVATOR AircraftInfo.MaxPitchElevator

void UAircraftVisualComponent::PitchCalculation(float D) 
{
	if (bSlats) 
	{
		if (InputPitch <= 0) Slat = FMath::FInterpTo(Slat, 0, D, 4);
		else Slat = FMath::FInterpTo(Slat, FMath::Clamp(Slat + InputPitch, 0, MAXSLATS), D, INTERPSPEED);
	}
	if (bElevators) 
	{
		if (InputPitch == 0) ElevatorPitch = FMath::FInterpTo(ElevatorPitch, 0, D, INTERPSPEED);
		else ElevatorPitch = FMath::FInterpTo(ElevatorPitch, FMath::Clamp(ElevatorPitch + InputPitch, -MAXPITCHELEVATOR, MAXPITCHELEVATOR), D, INTERPSPEED);
	}
	if (bCanards) 
	{
		// TODO: Once there are canard aircraft add something for this
	}
}

#define MAXRUDDER AircraftInfo.MaxRudder

void UAircraftVisualComponent::YawCalculation(float D) 
{
	if (InputYaw == 0) Rudder = FMath::FInterpTo(Rudder, 0, D, INTERPSPEED);
	else Rudder = FMath::FInterpTo(Rudder, FMath::Clamp(Rudder + InputYaw, -MAXRUDDER, MAXRUDDER), D, INTERPSPEED);
}

#define MAXROLLFLAPS AircraftInfo.MaxRollFlaps
#define MAXELEVATORROLL AircraftInfo.MaxRollElevator

void UAircraftVisualComponent::RollCalculation(float D) 
{
	if (InputRoll == 0) FlapRoll = FMath::FInterpTo(FlapRoll, 0, D, INTERPSPEED);
	else FlapRoll = FMath::FInterpTo(FlapRoll, FMath::Clamp(FlapRoll + InputRoll, -MAXROLLFLAPS, MAXROLLFLAPS), D, INTERPSPEED);
	if (bElevators)
	{
		if (InputRoll == 0) ElevatorRoll = FMath::FInterpTo(ElevatorRoll, 0, D, INTERPSPEED);
		else ElevatorRoll = FMath::FInterpTo(ElevatorRoll, FMath::Clamp(ElevatorRoll + InputRoll, -MAXELEVATORROLL, MAXELEVATORROLL), D, INTERPSPEED);
	}
}

#define AFTERBURNERPERCENT 0.8
#define MINNOZZLE AircraftInfo.MinExhaust
#define MAXNOZZLE AircraftInfo.MaxExhaust

void UAircraftVisualComponent::ThrustCalculation(float D) 
{
	if (bNozzle) 
	{
		if (0 <= InputThrust && InputThrust <= AFTERBURNERPERCENT) 
		{
			float reverse = InputThrust / 2;
			Nozzle = FMath::FInterpTo(Nozzle, FMath::Clamp(1 - reverse, MINNOZZLE, MAXNOZZLE), D, INTERPSPEED);
		}
		else 
		{
			float Ratio = (InputThrust - AFTERBURNERPERCENT) / 0.2;
			Nozzle = FMath::FInterpTo(Nozzle, FMath::Clamp(MAXNOZZLE * Ratio, MINNOZZLE, MAXNOZZLE), D, INTERPSPEED);
		}
	}
}

#define SLOWSPEED 0.4
#define MAXAIRBRAKE AircraftInfo.MaxAirbrake

void UAircraftVisualComponent::AirBrakeCalculation(float D)
{
	if (InputThrust > SLOWSPEED) AirBrake = FMath::FInterpTo(AirBrake, 0, D, INTERPSPEED);
	else AirBrake = FMath::FInterpTo(AirBrake, MAXAIRBRAKE, D, INTERPSPEED);
}