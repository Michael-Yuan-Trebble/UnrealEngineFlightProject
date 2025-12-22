// Fill out your copyright notice in the Description page of Project Settings.


#include "Aircraft/Player/Mirage2000Pawn.h"
#include "Aircraft/FlightComponent.h"
#include "AircraftPlayerController.h"

#define SLATMAX 10
#define PITCHFLAPMAX 4
#define FLAPROLLMAX 20
#define FLAPPITCHMIN 1
#define RUDDERMAX 20
#define AIRBRAKEMAX 70
#define EXHAUSTMINSCALE 0.7
#define EXHAUSTMAXSCALE 1
#define INTERPSPEED 50
#define SLOWINTERP 10

AMirage2000Pawn::AMirage2000Pawn() {

}

void AMirage2000Pawn::PossessedBy(AController* NewController) {
	Super::PossessedBy(NewController);
	Controlled = Cast <AAircraftPlayerController>(NewController);
}

void AMirage2000Pawn::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	if (!Controlled || !FlightComponent) return;
	InputPitchValue = FlightComponent->UserPitch;
	InputYawValue = FlightComponent->UserYaw;
	InputRollValue = FlightComponent->UserRoll;
	InputThrust = FlightComponent->CurrentThrust;
	PitchCalculation(DeltaTime);
	RollCalculation(DeltaTime);
	YawCalculation(DeltaTime);
	ThrustCalculation(DeltaTime);
	AirBrakeCalculation(DeltaTime);
}

void AMirage2000Pawn::PitchCalculation(float Delta) {
	if (InputPitchValue <= 0) 
	{
		Slat = FMath::FInterpTo(Slat, 0, Delta, 4);
	}
	else if (InputPitchValue > 0) 
	{
		Slat = FMath::FInterpTo(Slat, FMath::Clamp(Slat + InputPitchValue,0,SLATMAX), Delta, INTERPSPEED);
	}
}

void AMirage2000Pawn::RollCalculation(float Delta) {
	if (InputRollValue == 0) 
	{
		TotalFlapRoll = FMath::FInterpTo(TotalFlapRoll, 0, Delta, 4);
	}
	else 
	{
		TotalFlapRoll = FMath::FInterpTo(TotalFlapRoll, FMath::Clamp(TotalFlapRoll + InputRollValue, -FLAPROLLMAX, FLAPROLLMAX), Delta, INTERPSPEED);
	}

	if (InputPitchValue == 0) {
		TotalFlapPitch = FMath::FInterpTo(TotalFlapPitch, 0, Delta, 4);
	}
	else {
		TotalFlapPitch = FMath::FInterpTo(TotalFlapPitch, FMath::Clamp(TotalFlapPitch + InputRollValue, -FLAPPITCHMIN, PITCHFLAPMAX), Delta, INTERPSPEED);
	}
	float Total = TotalFlapPitch + TotalFlapRoll;
	Flap = FMath::FInterpTo(Flap, Total, Delta, 4);
}

void AMirage2000Pawn::YawCalculation(float Delta) {
	if (InputYawValue == 0) {
		Rudder = FMath::FInterpTo(Rudder, 0, Delta, 4);
	}
	else{
		Rudder = FMath::FInterpTo(Rudder, FMath::Clamp(Rudder + InputYawValue, -RUDDERMAX, RUDDERMAX), Delta, INTERPSPEED);
	}
}

void AMirage2000Pawn::AirBrakeCalculation(float Delta) {
	if (InputThrust > 0.4) {
		TopAirBrake = FMath::FInterpTo(TopAirBrake, 0, Delta, SLOWINTERP);
		BottomAirbrake = FMath::FInterpTo(BottomAirbrake, 0, Delta, SLOWINTERP);
	}
	else {
		TopAirBrake = FMath::FInterpTo(TopAirBrake, AIRBRAKEMAX, Delta, SLOWINTERP);
		BottomAirbrake = FMath::FInterpTo(BottomAirbrake, -AIRBRAKEMAX, Delta, SLOWINTERP);
	}
}

void AMirage2000Pawn::ThrustCalculation(float Delta) {
	if (InputThrust >= 0 && InputThrust <= 0.8) {
		float reverse = InputThrust / 2;
		Nozzle = FMath::FInterpTo(Nozzle, FMath::Clamp(1-reverse, EXHAUSTMINSCALE, EXHAUSTMAXSCALE), Delta, SLOWINTERP);
	}
	else {
		float Ratio = (InputThrust - 0.8) / 0.2;
		Nozzle = FMath::FInterpTo(Nozzle, FMath::Clamp(EXHAUSTMAXSCALE * Ratio, EXHAUSTMINSCALE, EXHAUSTMAXSCALE), Delta, SLOWINTERP);
	}
}