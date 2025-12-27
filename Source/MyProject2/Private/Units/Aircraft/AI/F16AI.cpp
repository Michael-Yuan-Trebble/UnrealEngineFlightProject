// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("AI!"));
#include "Units/Aircraft/AI/F16AI.h"
#include "GameFramework/Pawn.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Units/Aircraft/BaseAircraft.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"

//Initialize F16AI
AF16AI::AF16AI()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AF16AI::BeginPlay()
{
	Super::BeginPlay();
}

void AF16AI::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AF16AI::ApplySpeed(float ThrottlePercentage) {

}

void AF16AI::RollToTarget(float RollInput, float DeltaSeconds) 
{
	//Application of Roll
	//currentRollAI = FMath::FInterpTo(currentRollAI, RollInput, DeltaSeconds, 8.f);

	//Roll in DeltaSeconds Terms
	//float DeltaRoll = -(currentRollAI * RollRate)* DeltaSeconds;
	//DeltaRoll = FMath::Clamp(DeltaRoll, -RollRate * DeltaSeconds, RollRate * DeltaSeconds);

	//FQuat DeltaRot = FQuat(FRotator(0, 0, DeltaRoll));

	//this->AddActorLocalRotation(DeltaRot);
}

void AF16AI::PitchToTarget(float PitchInput, float DeltaSeconds) 
{
	//float MaxDeltaThisFrame = ListedTurnRate * DeltaSeconds;
	//float TargetPitch = GetActorRotation().Pitch + PitchInput;

	//float SmoothPitch = FMath::FInterpTo(GetActorRotation().Pitch, TargetPitch, DeltaSeconds, 8.f);

	//float PitchStep = FMath::Clamp(SmoothPitch, GetActorRotation().Pitch - MaxDeltaThisFrame, GetActorRotation().Pitch + MaxDeltaThisFrame);

	//float PitchDelta = PitchStep - GetActorRotation().Pitch;

	//FRotator DeltaRot(0, 0, PitchDelta);
	//this->SetActorRotation(DeltaRot);
}

FDetectedAircraftInfo* AF16AI::ReturnTargeting() 
{
	return Targeting;
}

void AF16AI::PitchElevatorCalculation(float DeltaSeconds) {

}

void AF16AI::RollElevatorCalculation(float DeltaSeconds) {

}

void AF16AI::SlatsElevationCalculation(float DeltaSeconds) {

}

void AF16AI::RudderYawCalculation(float DeltaSeconds) {

}

void AF16AI::FlapPitchCalculation(float DeltaSeconds) {

}