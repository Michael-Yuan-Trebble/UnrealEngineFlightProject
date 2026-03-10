// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/Service/ChooseTarget.h"
#include "AI/AircraftAIController.h"
#include "Units/Aircraft/BaseAircraft.h"
#include "Units/Components/Aircraft/RadarComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Debug/DebugHelper.h"

UBTServiceChooseTarget::UBTServiceChooseTarget() {
	NodeName = "Update Target Actor";
	Interval = 0.5f;
	bNotifyBecomeRelevant = true;
	bNotifyCeaseRelevant = true;
	bNotifyTick = true;
}

void UBTServiceChooseTarget::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) {
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);
	OwnerComp.GetWorld()->GetTimerManager().SetTimerForNextTick(
		FTimerDelegate::CreateUObject(this, &UBTServiceChooseTarget::InitAfterTick, &OwnerComp)
	);
}

void UBTServiceChooseTarget::InitAfterTick(UBehaviorTreeComponent* OwnerComp) {
	if (!IsValid(OwnerComp)) return;
	AAircraftAIController* Controller = Cast<AAircraftAIController>(OwnerComp->GetAIOwner());
	if (!IsValid(Controller)) return;
	Controlled = Controller->GetControlled();
	BlackboardComponent = OwnerComp->GetBlackboardComponent();
}

void UBTServiceChooseTarget::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) {
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	if (!IsValid(Controlled) || !IsValid(BlackboardComponent)) return;
	timeSinceLastPick += DeltaSeconds;

	// TODO: Add a system where its not just the highest threat is picked, some sort of internal timer for changing targets
	AllAircraft = Controlled->GetRadarComp()->GetEnemies();
	if (!Selected.CurrentPawn.IsValid() || timeSinceLastPick >= PickInterval) 
	{
		PickTarget();
		timeSinceLastPick = 0.f;
		if(Selected.CurrentPawn.IsValid()) 
		{
			BlackboardComponent->SetValueAsObject(TargetActorKey.SelectedKeyName, Selected.CurrentPawn.Get());
		}
		else {
			BlackboardComponent->ClearValue(TargetActorKey.SelectedKeyName);
		}
	}
}

void UBTServiceChooseTarget::PickTarget() 
{
	for (FDetectedAircraftInfo& Info : AllAircraft)
	{
		if (Info.CurrentPawn.Get() == Controlled) return;
		Info.threatLevel = Info.CalculateThreat();
		if (!Selected.CurrentPawn.IsValid()) 
		{
			Selected = Info;
			continue;
		}

		if (Info.threatLevel > Selected.threatLevel) {
			Selected = Info;
		}
	}
}