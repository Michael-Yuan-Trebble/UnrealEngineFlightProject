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
			BlackboardComp->SetValueAsFloat(TEXT("ThrottleAmount"), 0.f);
		}
	}

	Controlled = Cast<AEnemyAircraft>(GetPawn());
	if (!Controlled) return;

}

void AEnemyAircraftAI::Tick(float DeltaTime) 
{
	Super::Tick(DeltaTime);
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
