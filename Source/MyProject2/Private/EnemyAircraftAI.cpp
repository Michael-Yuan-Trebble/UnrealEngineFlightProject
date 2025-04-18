// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Fuck!"));
#include "EnemyAircraftAI.h"
#include "F16AI.h"
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
	GetWorldTimerManager().SetTimer(RadarScanTimer, this, &AEnemyAircraftAI::Retrieve, 0.5f, true);
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

	if (Tracking.CurrentPawn) 
	{
		TrackingPawn = Tracking.CurrentPawn;
		TrackingLocation = TrackingPawn->GetActorLocation();
		TrackingRotation = Tracking.Rotation;
		TrackingDistance = Tracking.Distance;

		RotationTarget(DeltaTime);
	}

	//Need to Implement Radar Stuff first in order to confirm that tracking works

	//currentSpeed = FMath::FInterpTo(prevSpeed, currentSpeed, DeltaTime, 2.f);
	//vectorLocation = (currentSpeed)*Controlled->GetActorForwardVector();
}

void AEnemyAircraftAI::Retrieve() 
{
	Tracking = Controlled->ReturnTargeting();
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

//Only Roll to Target
void AEnemyAircraftAI::RotationTarget(float DeltaTime) 
{
	//Find the current up Vector of AI, then find out how much roll needed to match that up vector toward the plane (User)	
	FVector CurrentUp = Controlled->GetActorUpVector();
	FVector Forward = Controlled->GetActorForwardVector();
	FVector TargetUp = (TrackingLocation - Controlled->GetActorLocation()).GetSafeNormal();

	FVector NeedVector = TargetUp - (FVector::DotProduct(TargetUp, Forward) * Forward);
	NeedVector.Normalize();

	float RollAmount = FVector::DotProduct(FVector::CrossProduct(CurrentUp, NeedVector), Forward);
	RollAmount = FMath::Clamp(RollAmount, -1.f, 1.f);

	if (GEngine)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("Vector: %s"), *NeedVector.ToString()));
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("Vector: %f"), RollAmount));
	}

	//Add Roll to AI Plane Function
	Controlled->RollToTarget(RollAmount, DeltaTime);
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