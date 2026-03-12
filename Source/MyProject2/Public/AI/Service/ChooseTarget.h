// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "Structs and Data/InGame/FDetectedAircraftInfo.h"
#include "ChooseTarget.generated.h"

class ABaseAircraft;

UCLASS(Blueprintable, BlueprintType, meta = (DisplayName = "Tracking Service"))
class MYPROJECT2_API UBTServiceChooseTarget : public UBTService
{
	GENERATED_BODY()
	
public:
	UBTServiceChooseTarget();

private:
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UFUNCTION()
	void InitAfterTick(UBehaviorTreeComponent* OwnerComp);

	FDetectedAircraftInfo PickTarget(TArray<FDetectedAircraftInfo> AllAir, ABaseAircraft* Controlled);

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetActorKey;

	float timeSinceLastPick = 0.f;

	// TODO: Make the pick interval based on pilot skill, what they are currently doing, and random chance
	float PickInterval = 3.f;
};
