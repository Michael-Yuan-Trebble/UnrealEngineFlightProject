// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTServiceWeaponAngle.generated.h"

class ABaseAircraft;

UCLASS(Blueprintable, BlueprintType, meta = (DisplayName = "Weapon Angle Service"))
class MYPROJECT2_API UBTServiceWeaponAngle : public UBTService
{
	GENERATED_BODY()

public:

	UBTServiceWeaponAngle();

private:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetActorKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector bFireMissile;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector MissileClass;

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	void WeaponDistance(ABaseAircraft* Controlled, AActor* Selected, UBlackboardComponent* BlackboardComp);
};
