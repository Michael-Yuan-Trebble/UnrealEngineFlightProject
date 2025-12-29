// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Units/Aircraft/WeaponSystemComponent.h"
#include "Units/Aircraft/FlightComponent.h"
#include "BTTaskFireMissiles.generated.h"

class AEnemyAircraftAI;

UCLASS(Blueprintable)
class MYPROJECT2_API UBTTaskFireMissiles : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTTaskFireMissiles();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds);

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetActorKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector bFireMissile;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector MissileClass;

	UPROPERTY()
	UBlackboardComponent* BlackboardComp;

	UPROPERTY()
	AEnemyAircraftAI* Controller;

	UPROPERTY()
	UFlightComponent* FlightComp;

	UPROPERTY()
	AActor* Selected;

private:
	void EquipAppropriateWeapon();
};