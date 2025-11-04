// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "Aircraft/BaseAircraft.h"
#include "Attack.generated.h"

class AEnemyAircraftAI;
class AEnemyAircraft;

UCLASS(Blueprintable, BlueprintType, meta = (DisplayName="Attack Service"))
class MYPROJECT2_API UBTServiceAttack : public UBTService
{
	GENERATED_BODY()
	
public:

	UBTServiceAttack();

protected:

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetActorKey;
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	AActor* Selected;

	AEnemyAircraft* Controlled;

	UBlackboardComponent* BlackboardComp;

	AEnemyAircraftAI* Controller;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector YawKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector PitchKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector RollKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector ThrottleKey;

	float LastRoll = 0.f;

	const float PitchGain = 0.02f;

	const float YawGain = 0.02f;

	const float RollGain = 0.015f;

private:
	void CalculateAngle(float DeltaSeconds);

	float CalculateRollDegrees(FVector LocalDir);

	float CalculatePitchDegrees(FVector LocalDir);

	float CalculateYawDegrees(FVector LocalDir);

	void CalculateThrust(float DeltaSeconds);
};
