// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Service/BTServiceWeaponAngle.h"
#include "Aircraft/WeaponSystemComponent.h"
#include "Aircraft/FlightComponent.h"
#include "Aircraft/AI/EnemyAircraft.h"
#include "Aircraft/AI/EnemyAircraftAI.h"
#include "Weapons/BaseWeapon.h"

UBTServiceWeaponAngle::UBTServiceWeaponAngle() {
	NodeName = "Update Weapon Angle";
	Interval = 2.f;
	bNotifyBecomeRelevant = true;
}

void UBTServiceWeaponAngle::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) 
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);
	BlackboardComp = OwnerComp.GetBlackboardComponent();
	Selected = Cast<AActor>(BlackboardComp->GetValueAsObject(TargetActorKey.SelectedKeyName));

	for (const TPair<TSubclassOf<ABaseWeapon>, TArray<FCooldownWeapon* >>& GroupPair : Controlled->WeaponComponent->WeaponGroups)
	{
		TSubclassOf<ABaseWeapon> WeaponClass = GroupPair.Key;
		const TArray<FCooldownWeapon*>& WeaponArray = GroupPair.Value;
		float WeaponRange = WeaponArray[0]->WeaponInstance->range;

		if (WeaponRange > greatestRange)
		{
			greatestRange = WeaponRange;
		}
	}
}

void UBTServiceWeaponAngle::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) 
{
	if (!Controlled || !Controlled->WeaponComponent || !BlackboardComp || !Selected) return;
	WeaponDistance();
}

void UBTServiceWeaponAngle::WeaponDistance() 
{
	float Distance = FVector::Dist(Controlled->GetActorLocation(), Selected->GetActorLocation());
	if (Distance > greatestRange) return;

	if (!Controlled->WeaponComponent->bLocked) return;

	ABaseWeapon* CurrentWeapon = Controlled->WeaponComponent->CurrentWeapon;
	if (!CurrentWeapon) return;

	TSubclassOf<ABaseWeapon> FiringWeapon = nullptr;
	float smallestRange = Distance;

	for (const TPair<TSubclassOf<ABaseWeapon>, TArray<FCooldownWeapon* >> &GroupPair : Controlled->WeaponComponent->WeaponGroups)
	{
		TSubclassOf<ABaseWeapon> WeaponClass = GroupPair.Key;
		const TArray<FCooldownWeapon*>& WeaponArray = GroupPair.Value;

		float WeaponRange = WeaponArray[0]->WeaponInstance->range;

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