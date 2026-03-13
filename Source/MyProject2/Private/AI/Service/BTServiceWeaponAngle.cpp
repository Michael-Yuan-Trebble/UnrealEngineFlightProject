// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/Service/BTServiceWeaponAngle.h"
#include "Units/Components/Aircraft/WeaponSystemComponent.h"
#include "Structs and Data/InGame/CooldownWeapon.h"
#include "Units/Aircraft/BaseAircraft.h"
#include "AI/AircraftAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTServiceWeaponAngle::UBTServiceWeaponAngle() 
{
	NodeName = "Update Weapon Angle";
	Interval = 2.f;
	bNotifyBecomeRelevant = true;
}

void UBTServiceWeaponAngle::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) 
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AAircraftAIController* Controller = Cast<AAircraftAIController>(OwnerComp.GetAIOwner());
	if (!Controller) return;

	ABaseAircraft* Controlled = Cast<ABaseAircraft>(Controller->GetPawn());
	UBlackboardComponent* Comp = OwnerComp.GetBlackboardComponent();
	AActor* Selected = Cast<AActor>(Comp->GetValueAsObject(TargetActorKey.SelectedKeyName));

	WeaponDistance(Controlled, Selected, Comp);
}

void UBTServiceWeaponAngle::WeaponDistance(ABaseAircraft* Controlled, AActor* Selected, UBlackboardComponent* BlackboardComp)
{
	if (!IsValid(Selected) || !IsValid(Controlled)) return;
	UWeaponSystemComponent* WeaponComp = Controlled->GetWeaponComp();
	if (!IsValid(WeaponComp) || !WeaponComp->GetLocked()) return;

	float Distance = FVector::Dist(Controlled->GetActorLocation(), Selected->GetActorLocation());
	FCooldownWeapon* Found = WeaponComp->GetBestWeaponRange(Distance);
	
	if (!IsValid(BlackboardComp)) return;

	if (!Found && !Found->WeaponClass->IsChildOf(ABaseMissile::StaticClass())) {
		BlackboardComp->SetValueAsBool(bFireMissile.SelectedKeyName, false);
		return;
	}

	BlackboardComp->SetValueAsBool(bFireMissile.SelectedKeyName, true);
	BlackboardComp->SetValueAsClass(MissileClass.SelectedKeyName, Found->WeaponClass);
}