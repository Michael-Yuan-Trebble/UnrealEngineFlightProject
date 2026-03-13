// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTaskFireMissiles.generated.h"

UCLASS(Blueprintable)
class MYPROJECT2_API UBTTaskFireMissiles : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTTaskFireMissiles();

protected:
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds);

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetActorKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector bFireMissile;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector MissileClass;

private:
	void EquipAppropriateWeapon();
};