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

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetActorKey;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds);

	bool IsInsideCone(AActor* Player, float ConeDegrees);
	bool CanShotHit(AActor* Player);

	UBlackboardComponent* BlackboardComp;

	AEnemyAircraftAI* Controller;

	AActor* Selected;
};
