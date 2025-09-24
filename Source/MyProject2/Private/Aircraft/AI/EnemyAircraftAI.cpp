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
	power = (log10(20 / (0.07 * 1.225))) / (log10(maxSpeed));

}

void AEnemyAircraftAI::Tick(float DeltaTime) 
{
	Super::Tick(DeltaTime);
}

void AEnemyAircraftAI::ShouldYaw() 
{
	// Find the angle and distance between target and self to see if yaw is needed

	//FVector TargetDistance = TrackingLocation - Controlled->GetActorLocation();
	//float LengthDistance = TargetDistance.Length();
	//TargetDistance.Normalize();

	//float DotProduct = FVector::DotProduct(Controlled->GetActorForwardVector(), TargetDistance);

	//float ConeAngleCosine = FMath::Acos(DotProduct) * (180.f/PI);

	// TODO: Make angle variable between aircraft
	//if ((ConeAngleCosine <= 30.f) && (LengthDistance <= 100.f)) 
	//{
	//	bUseYaw = true;
	//}
	//else 
	//{
	//	bUseYaw = false;
	//}
}

void AEnemyAircraftAI::Retrieve() 
{
	//Tracking = Controlled->ReturnTargeting();
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
