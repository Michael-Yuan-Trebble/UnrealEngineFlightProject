// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("AI CONTROL!"));
#include "Aircraft/AI/EnemyAircraftAI.h"
#include "Aircraft/AI/F16AI.h"
#include "Engine/World.h"
#include "AircraftPlayerController.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Actor.h"
#include "Kismet/KismetMathLibrary.h"
#include "AircraftRegistry.h"
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
	DrawDebugCone(
		GetWorld(),
		Controlled->GetActorLocation(),
		Controlled->GetActorForwardVector(),
		10000.f,
		FMath::DegreesToRadians(40.f),
		FMath::DegreesToRadians(10.0f),
		12,
		FColor::Green,
		false,
		50.0f
	);
	GetWorldTimerManager().SetTimer(RadarScanTimer, this, &AEnemyAircraftAI::Retrieve, 0.5f, true);
	power = (log10(20 / (0.07 * 1.225))) / (log10(maxSpeed));

}

void AEnemyAircraftAI::Tick(float DeltaTime) 
{
	Super::Tick(DeltaTime);

	currentPitch = Controlled->GetActorRotation().Pitch;
	currentYaw = Controlled->GetActorRotation().Yaw;
	currentRoll = Controlled->GetActorRotation().Roll;

	if (!Controlled) return;

	Controlled->ApplySpeed(0.5);

	if (Tracking.CurrentPawn) 
	{
		TrackingPawn = Tracking.CurrentPawn;
		TrackingLocation = TrackingPawn->GetActorLocation();
		TrackingRotation = TrackingPawn->GetActorRotation();

		FVector DistanceWorld = TrackingLocation - Controlled->GetActorLocation();
		FVector LocalDistance = Controlled->GetActorTransform().InverseTransformVectorNoScale(DistanceWorld);

		//bUseYaw =  FMath::Abs(LocalDistance.Y) <= Controlled->YawBoxWidth * 0.5f && FMath::Abs(LocalDistance.Z) <= Controlled->YawBoxHeight;
		//LocalDistance.X <= Controlled->YawBoxDepth &&

		ShouldYaw();
		if (bUseYaw) 
		{
			print(text)
		}
		else 
		{
			RotationTarget(DeltaTime);
		}
		PitchTarget(DeltaTime);
	}
}

void AEnemyAircraftAI::ShouldYaw() 
{
	FVector TargetDistance = TrackingLocation - Controlled->GetActorLocation();
	float LengthDistance = TargetDistance.Length();
	TargetDistance.Normalize();

	float DotProduct = FVector::DotProduct(Controlled->GetActorForwardVector(), TargetDistance);

	float ConeAngleCosine = FMath::Acos(DotProduct) * (180.f/PI);

	//30 Degrees for now, will change with every aircraft probably
	if ((ConeAngleCosine <= 30.f) && (LengthDistance <= 100.f)) 
	{
		bUseYaw = true;
	}
	else {
		bUseYaw = false;
	}
}

void AEnemyAircraftAI::Retrieve() 
{
	Tracking = Controlled->ReturnTargeting();
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
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("Roll: %f"), RollAmount));
	}

	//Add Roll to AI Plane Function
	Controlled->RollToTarget(RollAmount, DeltaTime);
}

void AEnemyAircraftAI::PitchTarget(float DeltaTime)
{
	FVector DistanceBetween = (TrackingLocation - Controlled->GetActorLocation()).GetSafeNormal();
	FVector Forward = Controlled->GetActorForwardVector();
	FRotator CurrentRot = Controlled->GetActorRotation();
	FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(TrackingRotation, CurrentRot);
	float pitchDif = DeltaRot.Pitch;

	Controlled->PitchToTarget(pitchDif, DeltaTime);
	if (GEngine)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("Pitch: %f"), pitchDif));
	}
}

void AEnemyAircraftAI::Rudder() 
{

}

void AEnemyAircraftAI::Weapons() {

}

void AEnemyAircraftAI::WeaponsCooldown() {

}

void AEnemyAircraftAI::Special() {

}

void AEnemyAircraftAI::SpecialCooldown() {

}

void AEnemyAircraftAI::ShootStart() {

}

void AEnemyAircraftAI::ShootEnd() {

}

void AEnemyAircraftAI::Bullets() {

}

void AEnemyAircraftAI::TurnSpeedLoss() {

}
