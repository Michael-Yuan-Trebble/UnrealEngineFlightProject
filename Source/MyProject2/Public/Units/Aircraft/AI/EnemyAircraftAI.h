// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Structs and Data/FDetectedAircraftInfo.h"
#include "EnemyAircraftAI.generated.h"

class AEnemyAircraft;
class ABaseWeapon;
class UWeaponSystemComponent;

UCLASS()
class MYPROJECT2_API AEnemyAircraftAI : public AAIController
{
	GENERATED_BODY()
	
public:
	AEnemyAircraftAI();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void OnPossess(APawn* PawnPossess) override;

	UPROPERTY(EditAnywhere,Category = "AI")
	UBehaviorTree* BehaviorTreeAsset;

	UPROPERTY(EditAnywhere,Category="AI")
	UBlackboardComponent* BlackboardComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite);
	float currentSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	AEnemyAircraft* Controlled;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FDetectedAircraftInfo Tracking;

	UPROPERTY()
	UWeaponSystemComponent* WeaponComp;

	bool bIsFiring = false;
	
	void Weapons(TSubclassOf<ABaseWeapon> WeaponClass, AActor* Selected, float Speed);

public:
	// Setting Controller Values
	void SetThrust(float Thrust);

	void SetPitch(float Pitch);
	void SetRoll(float Raw);
	void SetYaw(float Yaw);

	void SetFlying(bool bIsFlying);

	void ShootGun(bool bShoot);
	void SwitchWeapon(TSubclassOf<ABaseWeapon> InWeapon);

private:
	void Special();

	void SpecialCooldown();

};
