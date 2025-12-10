// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTServiceShootGun.generated.h"

#define SHOOTDISTANCE 75000
#define CONEANGLE 5

class AEnemyAircraft;
class AEnemyAircraftAI;

UCLASS(Blueprintable, BlueprintType, meta = (DisplayName = "Shoot Gun Service"))
class MYPROJECT2_API UBTServiceShootGun : public UBTService
{
	GENERATED_BODY()
	
public:
	UBTServiceShootGun();

private:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetActorKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector bFireGun;

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	AActor* Selected;

	AEnemyAircraft* Controlled;

	AEnemyAircraftAI* Controller;

	UBlackboardComponent* BlackboardComp;

	bool IsInsideCone();
};
