// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/Task/BTTaskFireMissiles.h"
#include "AI/AircraftAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Weapons/BaseWeapon.h"

UBTTaskFireMissiles::UBTTaskFireMissiles()
{
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTaskFireMissiles::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);
	BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!IsValid(BlackboardComp)) return EBTNodeResult::Aborted;

	Controller = Cast<AAircraftAIController>(OwnerComp.GetAIOwner());
	if (!IsValid(Controller)) return EBTNodeResult::Aborted;

	Selected = Cast<AActor>(BlackboardComp->GetValueAsObject(TargetActorKey.SelectedKeyName));

	return EBTNodeResult::InProgress;
}

void UBTTaskFireMissiles::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	if (!IsValid(BlackboardComp)) return;
	if (!BlackboardComp->GetValueAsBool(bFireMissile.SelectedKeyName)) return;
	TSubclassOf<ABaseWeapon> Class = BlackboardComp->GetValueAsClass(MissileClass.SelectedKeyName);

	// TODO: Make it first select the class if not selected then fire, ithout using FlightComp
	if (!IsValid(Class)) return;
	//Controller->Weapons(Class, Selected, FlightComp->currentSpeed);
}

void UBTTaskFireMissiles::EquipAppropriateWeapon() {

}