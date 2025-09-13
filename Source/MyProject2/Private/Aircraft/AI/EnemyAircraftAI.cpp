// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("AI CONTROL!"));
#include "Aircraft/AI/EnemyAircraftAI.h"
#include "Aircraft/AI/F16AI.h"
#include "Engine/World.h"
#include "AircraftPlayerController.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "GameFramework/Actor.h"
#include "Aircraft/BaseAircraft.h"
#include "Kismet/KismetMathLibrary.h"
#include "AircraftRegistry.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"

AEnemyAircraftAI::AEnemyAircraftAI() 
{
	PrimaryActorTick.bCanEverTick = true;
}

void AEnemyAircraftAI::BeginPlay() 
{
	Super::BeginPlay();
	targetPitch = 0.f;
	targetYaw = 0.f;
	targetRoll = 0.f;

}

void AEnemyAircraftAI::OnPossess(APawn* PawnPossess) 
{
	Super::OnPossess(PawnPossess);

	if (BehaviorTreeAsset) 
	{
		if (UseBlackboard(BehaviorTreeAsset->BlackboardAsset, BlackboardComp)) 
		{
			RunBehaviorTree(BehaviorTreeAsset);
			BlackboardComp->SetValueAsBool(TEXT("bIsAttacking"), false);
			BlackboardComp->SetValueAsFloat(TEXT("PitchAmount"), 0.f);
			BlackboardComp->SetValueAsFloat(TEXT("RollAmount"), 0.f);
			BlackboardComp->SetValueAsFloat(TEXT("YawAmount"), 0.f);
		}
	}

	Controlled = Cast<AEnemyAircraft>(GetPawn());
	if (!Controlled) return;

	// Draw Radar Cone

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

	if (!Controlled) return;

	currentPitch = Controlled->GetActorRotation().Pitch;
	currentYaw = Controlled->GetActorRotation().Yaw;
	currentRoll = Controlled->GetActorRotation().Roll;

	// TODO: Move tracking logic into Blackboard instead of in the AI 
	// Prioritize Pitch and Roll, with Yaw only being used in a certain distance and in a certain cone around an area

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
	// Find the angle and distance between target and self to see if yaw is needed

	FVector TargetDistance = TrackingLocation - Controlled->GetActorLocation();
	float LengthDistance = TargetDistance.Length();
	TargetDistance.Normalize();

	float DotProduct = FVector::DotProduct(Controlled->GetActorForwardVector(), TargetDistance);

	float ConeAngleCosine = FMath::Acos(DotProduct) * (180.f/PI);

	// TODO: Make angle variable between aircraft
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
	//Tracking = Controlled->ReturnTargeting();
}

//Only Roll to Target
void AEnemyAircraftAI::RotationTarget(float DeltaTime) 
{
}

void AEnemyAircraftAI::PitchTarget(float DeltaTime)
{
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
