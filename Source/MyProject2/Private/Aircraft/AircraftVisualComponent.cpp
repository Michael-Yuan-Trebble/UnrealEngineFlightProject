// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Visual Comp!"));
#include "Aircraft/AircraftVisualComponent.h"
#include "Specials/CountermeasureActor.h"

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
	RFlap = FlapPitch + RFlapRoll;
	LFlap = FlapPitch + LFlapRoll;
}

void UAircraftVisualComponent::ActivateFlares() {
	if (!FlareClass || !Mesh) return;
	FName SocketName = FName(*FString::Printf(TEXT("CountermeasureSocket")));
	print(text)
	if (!Mesh->DoesSocketExist(SocketName)) return;
	print(text)
	ACountermeasureActor* FlareActor = GetWorld()->SpawnActor<ACountermeasureActor>(
		FlareClass,
		Mesh->GetSocketTransform(SocketName)
	);
	if (FlareActor) FlareActor->Activate();
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

	if (InputPitch <= 0) FlapPitch = FMath::FInterpTo(FlapPitch, 0, D, INTERPSPEED);
	else FlapPitch = FMath::FInterpTo(FlapPitch, FMath::Clamp(FlapPitch - InputPitch, -MAXPITCHFLAPS, -MINPITCHFLAPS), D, INTERPSPEED);

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
	if (InputRoll == 0) 
	{
		LFlapRoll = FMath::FInterpTo(LFlapRoll, 0, D, INTERPSPEED);
		RFlapRoll = FMath::FInterpTo(RFlapRoll, 0, D, INTERPSPEED);
	}
	else 
	{
		LFlapRoll = FMath::FInterpTo(LFlapRoll, FMath::Clamp(LFlapRoll + InputRoll, -MAXROLLFLAPS, MAXROLLFLAPS), D, INTERPSPEED);
		RFlapRoll = -LFlapRoll;
	}

	// Elevators have a slight pitch when rolling
	// TODO: Also separate the roll logic here

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
			float Ratio = (InputThrust - AFTERBURNERPERCENT) / (1 - AFTERBURNERPERCENT);
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