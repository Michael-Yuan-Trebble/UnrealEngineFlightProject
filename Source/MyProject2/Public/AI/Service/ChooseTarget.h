// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "Structs and Data/FDetectedAircraftInfo.h"
#include "ChooseTarget.generated.h"

class AEnemyAircraft;

UCLASS(Blueprintable, BlueprintType, meta = (DisplayName = "Tracking Service"))
class MYPROJECT2_API UBTServiceChooseTarget : public UBTService
{
	GENERATED_BODY()
	
public:
	UBTServiceChooseTarget();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	void PickTarget();

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetActorKey;

	TArray<FDetectedAircraftInfo> AllAircraft;

	FDetectedAircraftInfo Selected;

	AEnemyAircraft* Controlled;

	float timeSinceLastPick;

	float PickInterval;
};
