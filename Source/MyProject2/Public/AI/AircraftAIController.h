// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Structs and Data/InGame/FDetectedAircraftInfo.h"
#include "AircraftAIController.generated.h"

class ABaseAircraft;
class ABaseWeapon;
class UBehaviorTree;
class UBlackboardComponent;

UCLASS()
class MYPROJECT2_API AAircraftAIController : public AAIController
{
	GENERATED_BODY()

public:
	AAircraftAIController();

	void SetThrust(const float Thrust);

	void SetPitch(const float Pitch);
	void SetRoll(const float Raw);
	void SetYaw(const float Yaw);

	void SetFlying(const bool bIsFlying);

	void ShootGun(const bool bShoot);
	void SwitchWeapon(TSubclassOf<ABaseWeapon> InWeapon);

	void Special();

	void Weapons();

	ABaseAircraft* GetControlled() const { return Controlled; };

private:

	virtual void BeginPlay() override;

	virtual void Tick(float D) override;

	virtual void OnPossess(APawn* PawnPossess) override;

	UPROPERTY(EditAnywhere)
	float currentSpeed = 0.f;

	UPROPERTY()
	TObjectPtr<ABaseAircraft> Controlled = nullptr;

	FDetectedAircraftInfo Tracking;

	bool bIsFiring = false;

	UPROPERTY(EditAnywhere, Category = "AI")
	TObjectPtr<UBehaviorTree> BehaviorTreeAsset = nullptr;

	UPROPERTY(EditAnywhere, Category = "AI")
	UBlackboardComponent* BlackboardComp = nullptr;
};
