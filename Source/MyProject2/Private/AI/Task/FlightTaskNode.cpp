// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/Task/FlightTaskNode.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AI/AircraftAIController.h"
#include "Units/Aircraft/AI/EnemyAircraft.h"

UBTTaskFlightTaskNode::UBTTaskFlightTaskNode() 
{
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTaskFlightTaskNode::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) 
{
	Super::ExecuteTask(OwnerComp, NodeMemory);
	return EBTNodeResult::InProgress;
}

void UBTTaskFlightTaskNode::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) 
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	AAircraftAIController* Controller = Cast<AAircraftAIController>(OwnerComp.GetAIOwner());
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();

	if (!IsValid(Controller) || !IsValid(BlackboardComp)) return;

	float YawOffset = BlackboardComp->GetValueAsFloat(YawKey.SelectedKeyName);
	YawOffset = FMath::IsNearlyZero(YawOffset) ? 0.f : YawOffset;

	float PitchOffset = BlackboardComp->GetValueAsFloat(PitchKey.SelectedKeyName);
	PitchOffset = FMath::IsNearlyZero(PitchOffset) ? 0.f : PitchOffset;

	float RollOffset = BlackboardComp->GetValueAsFloat(RollKey.SelectedKeyName);
	RollOffset = FMath::IsNearlyZero(RollOffset) ? 0.f : RollOffset;

	float Throttle = BlackboardComp->GetValueAsFloat(ThrottleKey.SelectedKeyName);
	Throttle = FMath::IsNearlyZero(Throttle) ? 0.f : Throttle;

	Controller->SetPitch(PitchOffset);
	Controller->SetRoll(RollOffset);
	Controller->SetYaw(YawOffset);
	Controller->SetThrust(Throttle);

	bool bShootGun = BlackboardComp->GetValueAsBool(bFireGun.SelectedKeyName);
	Controller->ShootGun(bShootGun);
}