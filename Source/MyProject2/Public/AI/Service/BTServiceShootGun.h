// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTServiceShootGun.generated.h"

class ABaseAircraft;

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

	UPROPERTY()
	TWeakObjectPtr<AActor> Selected = nullptr;

	UPROPERTY()
	TObjectPtr<AActor> Controlled = nullptr;

	UPROPERTY()
	TObjectPtr<UBlackboardComponent> BlackboardComp = nullptr;

	bool IsInsideCone();

	static constexpr int ShootDistance = 75000;

	static constexpr int ConeAngle = 5;
};
