// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("AI Choose!"));
#include "AI/Service/ChooseTarget.h"
#include "Aircraft/AI/EnemyAircraftAI.h"
#include "Aircraft/BaseAircraft.h"
#include "Aircraft/AI/EnemyAircraft.h"
#include "Aircraft/BaseAircraft.h"
#include "Aircraft/RadarComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "Kismet/GameplayStatics.h"

UBTServiceChooseTarget::UBTServiceChooseTarget() {
	NodeName = "Update Target Actor";
	Interval = 0.5f;
	bNotifyBecomeRelevant = true;
	timeSinceLastPick = 0.f;
	//Make the pick interval based on pilot skill, what they are currently doing, and random chance
	PickInterval = 3.f;
}

void UBTServiceChooseTarget::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) {
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	AEnemyAircraftAI* Controller = Cast<AEnemyAircraftAI>(OwnerComp.GetAIOwner());
	if (!Controller) return;

	Controlled = Cast<AEnemyAircraft>(Controller->Controlled);
	if (!Controlled) return;
	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComponent) return;

	timeSinceLastPick += DeltaSeconds;

	// TODO: Add a system where its not just the highest threat is picked, some sort of internal timer for changing targets
	AllAircraft = Controlled->RadarComponent->Enemies;
	if (!Selected.CurrentPawn || timeSinceLastPick >= PickInterval) 
	{
		PickTarget();
		timeSinceLastPick = 0.f;
		if(Selected.CurrentPawn) 
		{
			BlackboardComponent->SetValueAsObject(TargetActorKey.SelectedKeyName, Selected.CurrentPawn);
		}
		else {
			BlackboardComponent->ClearValue(TargetActorKey.SelectedKeyName);
		}
	}
}

void UBTServiceChooseTarget::PickTarget() 
{
	for (FDetectedAircraftInfo Info : AllAircraft)
	{
		if (Info.CurrentPawn == Controlled) return;
		if (GEngine && Info.CurrentPawn)
		{
			FString PawnName = Info.CurrentPawn->GetName();  
		}
		Info.threatLevel = Info.CalculateThreat();
		if (!Selected.CurrentPawn) 
		{
			Selected = Info;
			continue;
		}

		if (Info.threatLevel > Selected.threatLevel) {
			Selected = Info;
		}
	}
}