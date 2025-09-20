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
	return EBTNodeResult::InProgress;
}

void UBTTaskFlightTaskNode::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) 
{
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp) return;

	AEnemyAircraftAI* Controller = Cast<AEnemyAircraftAI>(OwnerComp.GetAIOwner());
	if (!Controller) return;

	AEnemyAircraft* Controlled = Cast<AEnemyAircraft>(Controller->Controlled);

	UFlightComponent* FlightComp = Controlled->FlightComponent;

	float YawOffset = BlackboardComp->GetValueAsFloat(YawKey.SelectedKeyName);
	float PitchOffset = BlackboardComp->GetValueAsFloat(PitchKey.SelectedKeyName);
	float RollOffset = BlackboardComp->GetValueAsFloat(RollKey.SelectedKeyName);
	if (GEngine)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("Pitch: %f"), PitchOffset));
	}
	FlightComp->isFlying = true;
	//FlightComp->SetPitch(PitchOffset);
	FlightComp->SetRoll(RollOffset);
	FlightComp->ApplyRot(DeltaSeconds);
}