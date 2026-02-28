// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Task/BTTaskFireMissiles.h"
#include "Units/Aircraft/AI/EnemyAircraft.h"
#include "Units/Aircraft/AI/EnemyAircraftAI.h"
#include "Units/Aircraft/WeaponSystemComponent.h"
#include "Units/Aircraft/FlightComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Weapons/BaseWeapon.h"

UBTTaskFireMissiles::UBTTaskFireMissiles()
{
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTaskFireMissiles::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp) return EBTNodeResult::Aborted;

	Controller = Cast<AEnemyAircraftAI>(OwnerComp.GetAIOwner());
	if (!Controller) return EBTNodeResult::Aborted;

	AEnemyAircraft* Controlled = Cast<AEnemyAircraft>(Controller->Controlled);
	if (!Controlled) return EBTNodeResult::Aborted;

	FlightComp = Controlled->GetFlightComp();

	Selected = Cast<AActor>(BlackboardComp->GetValueAsObject(TargetActorKey.SelectedKeyName));

	return EBTNodeResult::InProgress;
}

void UBTTaskFireMissiles::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	if (!BlackboardComp->GetValueAsBool(bFireMissile.SelectedKeyName)) return;
	TSubclassOf<ABaseWeapon> Class = BlackboardComp->GetValueAsClass(MissileClass.SelectedKeyName);

	// TODO: Make it first select the class if not selected then fire, ithout using FlightComp
	if (!Class) return;
	//Controller->Weapons(Class, Selected, FlightComp->currentSpeed);
}