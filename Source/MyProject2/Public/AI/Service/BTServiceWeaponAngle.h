// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "Units/Aircraft/BaseAircraft.h"
#include "BTServiceWeaponAngle.generated.h"

class AEnemyAircraft;

UCLASS(Blueprintable, BlueprintType, meta = (DisplayName = "Weapon Angle Service"))
class MYPROJECT2_API UBTServiceWeaponAngle : public UBTService
{
	GENERATED_BODY()

public:

	UBTServiceWeaponAngle();

protected:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetActorKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector bFireMissile;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector MissileClass;

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY()
	AActor* Selected;

	UPROPERTY()
	AEnemyAircraft* Controlled;
	
	UPROPERTY()
	UBlackboardComponent* BlackboardComp;

private:
	void WeaponDistance();

	float greatestRange = 0.f;
};
