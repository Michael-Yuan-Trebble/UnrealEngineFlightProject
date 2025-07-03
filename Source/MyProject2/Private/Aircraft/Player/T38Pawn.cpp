// Fill out your copyright notice in the Description page of Project Settings.


#include "Aircraft/Player/T38Pawn.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "AircraftPlayerController.h"

AT38Pawn::AT38Pawn() {
	PrimaryActorTick.bCanEverTick = true;

	ListedAcceleration = 10.f;
	TakeoffSpeed = 30.f;
	RollRate = 10.f;
	ListedTurnRate = 0.3f;
	ListedMaximumSpeed = 10000.f;
	ListedThrust = 20.f;
	ListedRudder = 0.1f;
	springArmLength = 1800.f;
	NumPylons = 4;
	AOAReturnSpeed = 1.5;
}

void AT38Pawn::PossessedBy(AController* NewController) {
	Super::PossessedBy(NewController);
	Controlled = Cast<AAircraftPlayerController>(NewController);
	AddPylons();
	power = (log10(20 / (0.07 * 1.225))) / (log10(ListedMaximumSpeed));
}

void AT38Pawn::BeginPlay() {
	Super::BeginPlay();
}

void AT38Pawn::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);
	if (Controlled) {
		InputPitchValue = Controlled->inputPitch;
		InputThrust = Controlled->inputThrust;
		InputYawValue = Controlled->inputYaw;
		InputRollValue = Controlled->inputRoll;
		RollCalculation(DeltaSeconds);
		PitchCalculation(DeltaSeconds);
	}
}

void AT38Pawn::PitchCalculation(float DeltaSeconds) {
	if (InputPitchValue == 0) {
		LeftElevator = FMath::FInterpTo(LeftElevator, 0, DeltaSeconds, 4);
		RightElevator = FMath::FInterpTo(RightElevator, 0, DeltaSeconds, 4);
	}
	else {
		LeftElevator = FMath::Clamp((InputPitchValue)+LeftElevator, -20.f, 20.f);
		LeftElevator = FMath::FInterpTo(LeftElevator, InputPitchValue * 20.f, DeltaSeconds, 0.2);
		RightElevator = LeftElevator;
	}
}

void AT38Pawn::RollCalculation(float DeltaSeconds) {
	if (InputRollValue == 0) {
		LeftTurningFlap = FMath::FInterpTo(LeftTurningFlap, 0, DeltaSeconds, 2);
		RightTurningFlap = FMath::FInterpTo(RightTurningFlap, 0, DeltaSeconds, 2);
	}
	else {
		LeftTurningFlap = FMath::Clamp(LeftTurningFlap + InputRollValue, -40.f, 40.f);
		LeftTurningFlap = FMath::FInterpTo(LeftTurningFlap, InputRollValue * 40.f, DeltaSeconds, 0.2);
		RightTurningFlap = LeftTurningFlap;
	}
}

void AT38Pawn::YawCalculation(float DeltaSeconds) {
	if (InputYawValue == 0) {
		Rudder = FMath::FInterpTo(Rudder, 0, DeltaSeconds, 2);
	}
	else {
		Rudder = FMath::Clamp(InputYawValue + Rudder, -20, 20);
		Rudder = FMath::FInterpTo(Rudder, InputYawValue * 20, DeltaSeconds, 0.2);
	}
}

void AT38Pawn::AirBrakeCalculation(float DeltaSeconds) {
	if (InputThrust <= -0.9) {
		LeftAirBrake = FMath::FInterpTo(LeftAirBrake, -90, DeltaSeconds, 2.f);
		RightAirBrake = LeftAirBrake;
	}
	else {
		LeftAirBrake = FMath::FInterpTo(LeftAirBrake, 0, DeltaSeconds, 2.f);
		RightAirBrake = LeftAirBrake;
	}
}