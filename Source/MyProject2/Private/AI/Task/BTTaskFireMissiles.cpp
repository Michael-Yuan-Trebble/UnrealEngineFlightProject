// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/Task/BTTaskFireMissiles.h"
#include "AI/AircraftAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Weapons/BaseWeapon.h"

UBTTaskFireMissiles::UBTTaskFireMissiles()
{
	bNotifyTick = true;
}

void UBTTaskFireMissiles::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	AAircraftAIController* Controller = Cast<AAircraftAIController>(OwnerComp.GetAIOwner());
	if (!Controller) return;

	AActor* Controlled = Controller->GetPawn();
	UBlackboardComponent* Comp = OwnerComp.GetBlackboardComponent();
	AActor* Selected = Cast<AActor>(Comp->GetValueAsObject(TargetActorKey.SelectedKeyName));

	if (!IsValid(Comp)) return;
	if (!Comp->GetValueAsBool(bFireMissile.SelectedKeyName)) return;
	TSubclassOf<ABaseWeapon> Class = Comp->GetValueAsClass(MissileClass.SelectedKeyName);

	// TODO: Make it first select the class if not selected then fire, ithout using FlightComp
	if (!IsValid(Class)) return;
	//Controller->Weapons(Class, Selected, FlightComp->currentSpeed);
}

void UBTTaskFireMissiles::EquipAppropriateWeapon() {

}