// Fill out your copyright notice in the Description page of Project Settings.


#include "FlightTaskNode.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Aircraft/AI/EnemyAircraftAI.h"
#include "Aircraft/AI/EnemyAircraft.h"
#include "FlightComponent.h"

UBTTaskFlightTaskNode::UBTTaskFlightTaskNode() {

}

EBTNodeResult::Type UBTTaskFlightTaskNode::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) {
	return EBTNodeResult::InProgress;
}

void UBTTaskFlightTaskNode::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) {
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp) return;

	AEnemyAircraftAI* Controller = Cast<AEnemyAircraftAI>(OwnerComp.GetAIOwner());
	if (!Controller) return;

	AEnemyAircraft* Controlled = Cast<AEnemyAircraft>(Controller->Controlled);

	UFlightComponent* FlightComp = Controlled->FlightComponent;

	float YawOffset = BlackboardComp->GetValueAsFloat(YawKey.SelectedKeyName);
	float PitchOffset = BlackboardComp->GetValueAsFloat(PitchKey.SelectedKeyName);
	float RollOffset = BlackboardComp->GetValueAsFloat(RollKey.SelectedKeyName);

	FlightComp->ApplyRoll(YawOffset);
	FlightComp->ApplyPitch(PitchOffset);  
	FlightComp->ApplyRoll(RollOffset);
}