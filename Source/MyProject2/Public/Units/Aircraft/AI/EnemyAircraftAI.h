// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Structs and Data/FDetectedAircraftInfo.h"
#include "EnemyAircraftAI.generated.h"

class AEnemyAircraft;
class ABaseWeapon;
class UWeaponSystemComponent;
class UBehaviorTree;
class UBlackboardComponent;

UCLASS()
class MYPROJECT2_API AEnemyAircraftAI : public AAIController
{
	GENERATED_BODY()
	
public:
	AEnemyAircraftAI();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void OnPossess(APawn* PawnPossess) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite);
	float currentSpeed = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<AEnemyAircraft> Controlled = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FDetectedAircraftInfo Tracking;

	bool bIsFiring = false;
	
	void Weapons(TSubclassOf<ABaseWeapon> WeaponClass, AActor* Selected, float Speed);

public:
	// Setting Controller Values
	void SetThrust(const float Thrust);

	void SetPitch(const float Pitch);
	void SetRoll(const float Raw);
	void SetYaw(const float Yaw);

	void SetFlying(bool bIsFlying);

	void ShootGun(bool bShoot);
	void SwitchWeapon(TSubclassOf<ABaseWeapon> InWeapon);

private:
	void Special();

	void SpecialCooldown();

	UPROPERTY()
	TObjectPtr<UWeaponSystemComponent> WeaponComp = nullptr;

	UPROPERTY(EditAnywhere, Category = "AI")
	TObjectPtr<UBehaviorTree> BehaviorTreeAsset = nullptr;

	UPROPERTY(EditAnywhere, Category = "AI")
	UBlackboardComponent* BlackboardComp = nullptr;
};
