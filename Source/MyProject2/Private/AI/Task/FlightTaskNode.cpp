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
	if (!Controlled) return EBTNodeResult::Aborted;

	FlightComp = Controlled->FlightComponent;
	FlightComp->isFlying = true;
	return EBTNodeResult::InProgress;
}

void UBTTaskFlightTaskNode::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) 
{
	float YawOffset = BlackboardComp->GetValueAsFloat(YawKey.SelectedKeyName);
	YawOffset = FMath::IsNearlyZero(YawOffset) ? 0.f : YawOffset;

	float PitchOffset = BlackboardComp->GetValueAsFloat(PitchKey.SelectedKeyName);
	PitchOffset = FMath::IsNearlyZero(PitchOffset) ? 0.f : PitchOffset;

	float RollOffset = BlackboardComp->GetValueAsFloat(RollKey.SelectedKeyName);
	RollOffset = FMath::IsNearlyZero(RollOffset) ? 0.f : RollOffset;

	float Throttle = BlackboardComp->GetValueAsFloat(ThrottleKey.SelectedKeyName);
	Throttle = FMath::IsNearlyZero(Throttle) ? 0.f : Throttle;

	FlightComp->SetPitch(PitchOffset);
	FlightComp->SetRoll(RollOffset);
	FlightComp->SetYaw(YawOffset);
	FlightComp->ReturnAOA(DeltaSeconds);

	FlightComp->SetThrust(Throttle);
}