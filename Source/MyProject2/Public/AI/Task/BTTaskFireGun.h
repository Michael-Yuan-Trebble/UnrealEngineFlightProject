// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTaskFireGun.generated.h"

class AEnemyAircraftAI;

UCLASS(Blueprintable)
class MYPROJECT2_API UBTTaskFireGun : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTTaskFireGun();

private:

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetActorKey;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds);

	bool IsInsideCone(AActor* Self, AActor* Target);
	bool CanShotHit(AActor* Target, AActor* Self);

	static constexpr float ConeDegrees = 8.f;
};
