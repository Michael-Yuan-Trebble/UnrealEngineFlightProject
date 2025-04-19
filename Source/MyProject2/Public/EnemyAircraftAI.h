// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "AIController.h"
#include "FDetectedAircraftInfo.h"
#include "PID.h"
#include "EnemyAircraftAI.generated.h"

UCLASS()
class MYPROJECT2_API AEnemyAircraftAI : public AAIController
{
	GENERATED_BODY()
	
public:
	AEnemyAircraftAI();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void OnPossess(APawn* PawnPossess) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly);
	FRotator Target;

	UPROPERTY(EditAnywhere, BlueprintReadWrite);
	float currentSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class AF16AI* Controlled;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float planeAcceleration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBehaviorTree* BehaviorTree;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FDetectedAircraftInfo Tracking;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector vectorLocation;

	FVector TrackingLocation;
	FRotator TrackingRotation;
	APawn* TrackingPawn;
	float TrackingDistance;

protected:

	float targetPitch;

	float targetYaw;

	float targetRoll;

	float currentPitch;

	float currentYaw;

	float currentRoll;

	FTimerHandle RadarScanTimer;

private:

	void Retrieve();

	float maxSpeed;

	float thrust;

	float turnRate;

	float yawRate;

	float rollRate;

	float prevSpeed;

	float power;

	void Thrust(float Throttle);

	void Weapons();

	void WeaponsCooldown();

	void Special();

	void SpecialCooldown();

	void Rudder();

	void ShootStart();

	void ShootEnd();

	void Bullets();

	void TurnSpeedLoss();

	void RotationTarget(float DeltaTime);

	void PitchTarget(float DeltaTime);
};
