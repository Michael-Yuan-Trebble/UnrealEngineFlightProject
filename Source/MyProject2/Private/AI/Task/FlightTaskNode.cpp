// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Movement!"));

#include "AI/Task/FlightTaskNode.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Aircraft/AI/EnemyAircraftAI.h"
#include "Aircraft/AI/EnemyAircraft.h"
#include "Aircraft/FlightComponent.h"

UBTTaskFlightTaskNode::UBTTaskFlightTaskNode() 
{
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTaskFlightTaskNode::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) 
{
	BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp) return EBTNodeResult::Aborted;

	AEnemyAircraftAI* Controller = Cast<AEnemyAircraftAI>(OwnerComp.GetAIOwner());
	if (!Controller) return EBTNodeResult::Aborted;

	AEnemyAircraft* Controlled = Cast<AEnemyAircraft>(Controller->Controlled);

	FlightComp = Controlled->FlightComponent;
	FlightComp->isFlying = true;
	return EBTNodeResult::InProgress;
}

void UBTTaskFlightTaskNode::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) 
{
	float YawOffset = BlackboardComp->GetValueAsFloat(YawKey.SelectedKeyName);
	float PitchOffset = BlackboardComp->GetValueAsFloat(PitchKey.SelectedKeyName);
	float RollOffset = BlackboardComp->GetValueAsFloat(RollKey.SelectedKeyName);
	//FlightComp->SetPitch(PitchOffset);
	RollOffset = FMath::IsNearlyZero(RollOffset) ? 0.0f : RollOffset;
	FlightComp->SetRoll(RollOffset);
	FlightComp->ApplyRot(DeltaSeconds);
}