// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Visual Comp!"));
#include "Units/Aircraft/AircraftVisualComponent.h"
#include "Specials/CountermeasureActor.h"

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
	AircraftValues.Elevator = ElevatorPitch + ElevatorRoll;
	AircraftValues.RFlap = FlapPitch + RFlapRoll;
	AircraftValues.LFlap = FlapPitch + LFlapRoll;
}

void UAircraftVisualComponent::ActivateFlares() 
{
	if (!IsValid(FlareClass) || !IsValid(Mesh)) return;
	FName SocketName = FName(*FString::Printf(TEXT("CountermeasureSocket")));
	if (!Mesh->DoesSocketExist(SocketName)) return;
	ACountermeasureActor* FlareActor = GetWorld()->SpawnActor<ACountermeasureActor>(
		FlareClass,
		Mesh->GetSocketTransform(SocketName)
	);
	if (IsValid(FlareActor)) FlareActor->Activate();
}

void UAircraftVisualComponent::PitchCalculation(const float D) 
{
	// Leading Edge Slats

	if (bSlats) 
	{
		if (InputPitch <= 0) AircraftValues.Slat = FMath::FInterpTo(AircraftValues.Slat, 0, D, 4);
		else AircraftValues.Slat = FMath::FInterpTo(AircraftValues.Slat, FMath::Clamp(
			AircraftValues.Slat + InputPitch,
			0, 
			AircraftInfo.MaxSlats), 
			D, 
			InterpSpeed);
	}

	// Elevators pitch slightly
	// TODO: When delimiting AOA, elevators' max should be increased

	if (bElevators) 
	{
		if (InputPitch == 0) ElevatorPitch = FMath::FInterpTo(ElevatorPitch, 0, D, InterpSpeed);
		else ElevatorPitch = FMath::FInterpTo(
			ElevatorPitch, 
			FMath::Clamp(ElevatorPitch + InputPitch, 
				-AircraftInfo.MaxPitchElevator, 
				AircraftInfo.MaxPitchElevator), 
			D, 
			InterpSpeed);
	}

	if (InputPitch <= 0) FlapPitch = FMath::FInterpTo(FlapPitch, 0, D, InterpSpeed);
	else FlapPitch = FMath::FInterpTo(
		FlapPitch, 
		FMath::Clamp(FlapPitch - InputPitch, -AircraftInfo.MaxPitchFlaps, -AircraftInfo.MinPitchFlaps), 
		D, 
		InterpSpeed);

	if (bCanards) 
	{
		// TODO: Once there are canard aircraft add something for this
	}
}

void UAircraftVisualComponent::YawCalculation(const float D) 
{
	// Rudder
	if (InputYaw == 0) AircraftValues.Rudder = FMath::FInterpTo(AircraftValues.Rudder, 0, D, InterpSpeed);
	else AircraftValues.Rudder = FMath::FInterpTo(
		AircraftValues.Rudder,
		FMath::Clamp(AircraftValues.Rudder + InputYaw, -AircraftInfo.MaxRudder, AircraftInfo.MaxRudder),
		D, 
		InterpSpeed);
}

void UAircraftVisualComponent::RollCalculation(const float D)
{
	// Flaps roll equally
	if (InputRoll == 0) 
	{
		LFlapRoll = FMath::FInterpTo(LFlapRoll, 0, D, InterpSpeed);
		RFlapRoll = FMath::FInterpTo(RFlapRoll, 0, D, InterpSpeed);
	}
	else 
	{
		LFlapRoll = FMath::FInterpTo(
			LFlapRoll, 
			FMath::Clamp(LFlapRoll + InputRoll, -AircraftInfo.MaxRollFlaps, AircraftInfo.MaxRollFlaps), 
			D, 
			InterpSpeed);
		RFlapRoll = -LFlapRoll;
	}

	// Elevators have a slight pitch when rolling
	// TODO: Also separate the roll logic here

	if (bElevators)
	{
		if (InputRoll == 0) ElevatorRoll = FMath::FInterpTo(ElevatorRoll, 0, D, InterpSpeed);
		else ElevatorRoll = FMath::FInterpTo(
			ElevatorRoll, 
			FMath::Clamp(ElevatorRoll + InputRoll, -AircraftInfo.MaxRollElevator, AircraftInfo.MaxRollElevator), 
			D, 
			InterpSpeed);
	}
}

void UAircraftVisualComponent::ThrustCalculation(const float D)
{
	// All Nozzles contract and expand equally

	if (bNozzle) 
	{
		if (InputThrust <= 0.8)
		{
			float reverse = InputThrust / 2;
			AircraftValues.Nozzle = FMath::FInterpTo(
				AircraftValues.Nozzle,
				FMath::Clamp(1 - reverse, AircraftInfo.MinExhaust, AircraftInfo.MaxExhaust), 
				D, 
				InterpSpeed);
		}
		else 
		{
			float Ratio = (InputThrust - 0.8) / (1 - 0.8);
			AircraftValues.Nozzle = FMath::FInterpTo(
				AircraftValues.Nozzle,
				FMath::Clamp(AircraftInfo.MaxExhaust * Ratio, AircraftInfo.MinExhaust, AircraftInfo.MaxExhaust), 
				D, 
				InterpSpeed);
		}
	}
}

void UAircraftVisualComponent::AirBrakeCalculation(const float D)
{
	// Might move this also into thrust, depends if it doesn't make too much sense when expanded

	if (InputThrust > 0.4) AircraftValues.AirBrake = FMath::FInterpTo(AircraftValues.AirBrake, 0, D, InterpSpeed);
	else AircraftValues.AirBrake = FMath::FInterpTo(
		AircraftValues.AirBrake,
		AircraftInfo.MaxAirbrake, 
		D, 
		InterpSpeed);
}