// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Fuck!"));
#include "EnemyAircraftAI.h"
#include "F16AI.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "AircraftPlayerController.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Actor.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"

//Initialize AI
AEnemyAircraftAI::AEnemyAircraftAI() 
{
	PrimaryActorTick.bCanEverTick = true;
	BehaviorTree = nullptr;
}

void AEnemyAircraftAI::BeginPlay() 
{
	Super::BeginPlay();
	targetPitch = 0.f;
	targetYaw = 0.f;
	targetRoll = 0.f;
}

//Possession Function
void AEnemyAircraftAI::OnPossess(APawn* PawnPossess) 
{
	Super::OnPossess(PawnPossess);

	if (BehaviorTree) 
	{
		UBlackboardComponent* defaultBlackboard = nullptr;
		UseBlackboard(BehaviorTree->BlackboardAsset, defaultBlackboard);
		//RunBehaviorTree(BehaviorTree);
	}

	//Doesn't have Base AI Aircraft Class yet, will change the Cast and others based on different AI Airplane

	//Get Variables from AI
	Controlled = Cast<AF16AI>(GetPawn());
	maxSpeed = Controlled->ListedMaximumSpeed;
	planeAcceleration = Controlled->ListedAcceleration;
	thrust = Controlled->ListedThrust;
	turnRate = Controlled->ListedTurnRate;
	yawRate = Controlled->ListedRudder;
	rollRate = Controlled->RollRate;
	power = (log10(20 / (0.07 * 1.225))) / (log10(maxSpeed));

}

void AEnemyAircraftAI::Tick(float DeltaTime) 
{
	Super::Tick(DeltaTime);

	currentPitch = Controlled->GetActorRotation().Pitch;
	currentYaw = Controlled->GetActorRotation().Yaw;
	currentRoll = Controlled->GetActorRotation().Roll;

	if (Controlled) 
	{
		Thrust(0.5);
	}

	//Need to Implement Radar Stuff first in order to confirm that tracking works

	//currentSpeed = FMath::FInterpTo(prevSpeed, currentSpeed, DeltaTime, 2.f);
	//vectorLocation = (currentSpeed)*Controlled->GetActorForwardVector();

	for (FDetectedAircraftInfo* Detected : DetectedAircraft) 
	{
		Detected->Location = Detected->CurrentPawn->GetActorLocation();

		//Potentially don't need Rotation if its not gonna need it until its tracking
		//Detected->Rotation = Detected->CurrentActor->GetActorRotation();
		Detected->Distance = FVector::Dist(Controlled->GetActorLocation(), Detected->Location);
	}
	if (!Tracking.CurrentPawn && DetectedAircraft.Num() != 0) 
	{
		PickTarget();
	}

	if (DetectedAircraft.Num() == 0) 
	{
		Tracking.CurrentPawn = nullptr;
	}

	if (Tracking.CurrentPawn != nullptr)
	{
		if (Controlled) {
			Tracking.Location = Tracking.CurrentPawn->GetActorLocation();
			Tracking.Rotation = Tracking.CurrentPawn->GetActorRotation();
			Tracking.Distance = FVector::Dist(Controlled->GetActorLocation(), Tracking.Location);
			RotationTarget(DeltaTime);
		}
	}
}

void AEnemyAircraftAI::TriggerEvent() 
{

}

//Not Random
void AEnemyAircraftAI::PickTarget() 
{
	FDetectedAircraftInfo* Max = nullptr;
	for (FDetectedAircraftInfo* Detected : DetectedAircraft) 
	{
		if (Max == nullptr || Max->threatLevel < Detected->threatLevel)
		{
			Max = Detected;
		}
	}
	Tracking = *Max;
}

void AEnemyAircraftAI::UpdateAircraftControls(float DeltaTime) {
	
}

//Make the throttle like thrust percentage, currently not!!!
void AEnemyAircraftAI::Thrust(float Throttle) {


	prevSpeed = currentSpeed;
	if (Throttle < 0.f)
	{
		currentSpeed = currentSpeed - planeAcceleration;
		if (currentSpeed <= 0)
		{
			currentSpeed = 0;
		}
	}
	else
	{
		float currentThrust = planeAcceleration * Throttle;
		currentSpeed = currentSpeed + (currentThrust - ((0.5 * 0.07 * 1.225) * pow(prevSpeed, power)));
		if (currentSpeed >= maxSpeed)
		{
			currentSpeed = maxSpeed;
		}
	}
}

void AEnemyAircraftAI::RotationTarget(float DeltaTime) 
{
	
}

void AEnemyAircraftAI::Roll(float RollValue) 
{

}

void AEnemyAircraftAI::Pitch() 
{

}

void AEnemyAircraftAI::Rudder() 
{

}

/*
void Weapons();

void WeaponsCooldown();

void Special();

void SpecialCooldown();

void ShootStart();

void ShootEnd();

void Bullets();

void TurnSpeedLoss();
*/