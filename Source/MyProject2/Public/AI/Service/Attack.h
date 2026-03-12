// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "Attack.generated.h"

UENUM(BlueprintType)
enum class EAIThrottleMode : uint8 
{
	FarAway UMETA(DisplayName = "Far Away"),
	MidRange UMETA(DisplayName = "Mid Range"),
	Close UMETA(DisplayName = "Close")
};

class ABaseAircraft;

UCLASS(Blueprintable, BlueprintType, meta = (DisplayName="Attack Service"))
class MYPROJECT2_API UBTServiceAttack : public UBTService
{
	GENERATED_BODY()
	
public:

	UBTServiceAttack();

private:

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetActorKey;

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

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

	EAIThrottleMode GetThrottleMode(float distance);

	void CalculateAngle(const float DeltaSeconds, ABaseAircraft* Controlled, AActor* Selected, UBlackboardComponent* BlackboardComp);

	float CalculateRollDegrees(const FVector& LocalDir);

	float CalculatePitchDegrees(const FVector &LocalDir);

	float CalculateYawDegrees(const FVector& LocalDir);

	void CalculateThrust(const float DeltaSeconds, AActor* Selected, ABaseAircraft* Controlled, UBlackboardComponent* BlackboardComp);

	float PursuitThrottle(ABaseAircraft* Target, ABaseAircraft* Controlled);
};
