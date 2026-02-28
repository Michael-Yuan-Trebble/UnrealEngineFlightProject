// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Service/BTServiceWeaponAngle.h"
#include "Units/Aircraft/WeaponSystemComponent.h"
#include "Units/Aircraft/FlightComponent.h"
#include "Units/Aircraft/AI/EnemyAircraft.h"
#include "Units/Aircraft/AI/EnemyAircraftAI.h"
#include "Units/Aircraft/BaseAircraft.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Weapons/BaseWeapon.h"

UBTServiceWeaponAngle::UBTServiceWeaponAngle() 
{
	NodeName = "Update Weapon Angle";
	Interval = 2.f;
	bNotifyBecomeRelevant = true;
}

void UBTServiceWeaponAngle::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) 
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);
	BlackboardComp = OwnerComp.GetBlackboardComponent();
	Selected = Cast<AActor>(BlackboardComp->GetValueAsObject(TargetActorKey.SelectedKeyName));

	for (const TPair<TSubclassOf<ABaseWeapon>, TArray<FCooldownWeapon* >>& GroupPair : Controlled->GetWeaponComp()->GetWeaponGroups())
	{
		TSubclassOf<ABaseWeapon> WeaponClass = GroupPair.Key;
		const TArray<FCooldownWeapon*>& WeaponArray = GroupPair.Value;
		float WeaponRange = WeaponArray[0]->WeaponInstance->GetRange();

		if (WeaponRange > greatestRange)
		{
			greatestRange = WeaponRange;
		}
	}
}

void UBTServiceWeaponAngle::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) 
{
	if (!Controlled || !Controlled->GetWeaponComp() || !BlackboardComp || !Selected) return;
	WeaponDistance();
}

void UBTServiceWeaponAngle::WeaponDistance() 
{
	float Distance = FVector::Dist(Controlled->GetActorLocation(), Selected->GetActorLocation());
	if (Distance > greatestRange) return;

	if (!Controlled->GetWeaponComp()->GetLocked()) return;

	ABaseWeapon* CurrentWeapon = Controlled->GetWeaponComp()->GetWeapon();
	if (!CurrentWeapon) return;

	TSubclassOf<ABaseWeapon> FiringWeapon = nullptr;
	float smallestRange = Distance;

	for (const TPair<TSubclassOf<ABaseWeapon>, TArray<FCooldownWeapon* >> &GroupPair : Controlled->GetWeaponComp()->GetWeaponGroups())
	{
		TSubclassOf<ABaseWeapon> WeaponClass = GroupPair.Key;
		const TArray<FCooldownWeapon*>& WeaponArray = GroupPair.Value;

		float WeaponRange = WeaponArray[0]->WeaponInstance->GetRange();

		if (WeaponRange <= Distance)
		{
			if (WeaponRange < smallestRange)
			{
				FiringWeapon = WeaponClass;
				smallestRange = WeaponRange;
			}
		}
	}
	if (!FiringWeapon) return;

	BlackboardComp->SetValueAsBool(bFireMissile.SelectedKeyName, true);
	BlackboardComp->SetValueAsClass(MissileClass.SelectedKeyName, FiringWeapon);
}