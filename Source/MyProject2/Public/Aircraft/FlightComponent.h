// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Structs and Data/Aircraft Data/AircraftStats.h"
#include "Aircraft/BaseAircraft.h"
#include "Structs and Data/ThrottleStage.h"
#include "FlightComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAfterburnerEngaged, bool, isActive);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYPROJECT2_API UFlightComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	FOnAfterburnerEngaged OnAfterburnerEngaged;

	UFlightComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UAircraftStats* AircraftStats;

	ABaseAircraft* Controlled;

	bool isFlying = false;

	UPROPERTY(BlueprintReadOnly)
	float currentSpeed;

	float UserPitch;

	float UserYaw;

	float UserRoll;

	float NextPitch;

	float NextYaw;

	float NextRoll;

	float CurrentThrust;

	float targetSpeed;

	float Acceleration;
	
	float DownPitch;

	UPROPERTY(BlueprintReadOnly)
	int displayG = 1;

	FVector Velocity;

	FVector PreviousVelocity;
	
	float gForce;

	EThrottleStage prevStage = EThrottleStage::Slow;

	EThrottleStage currentStage = EThrottleStage::Normal;

	bool switchingPhase = false;

	void ApplySpeed(float ThrottlePercentage, float DeltaSeconds);

	void ApplyRot(float DletaSeconds);

	void ApplyPitch(float DeltaSeconds);

	void ApplyYaw(float DeltaSeconds);

	void ApplyRoll(float DeltaSeconds);

	void SlowSpeed(float ThrottlePercentage);

	void NormalSpeed(float ThrottlePercentage);

	void AfterburnerSpeed(float ThrottlePercentage);

	void ReturnAOA(float DeltaSeconds);

	void Setup(ABaseAircraft* InControl, UAircraftStats* InStats);

	void AdjustSpringArm(float DeltaSeconds, float ThrottlePercentage);

	void CalculateGForce(float DeltaSeconds);

	float DragAOA();

	void RollAOA(float DeltaSeconds);

	float PitchDrag();

	EThrottleStage getThrottleStage(float throttle);

	void SetPitch(float PitchValue) { UserPitch = PitchValue; };

	void SetYaw(float YawValue) { UserYaw = YawValue; };

	void SetRoll(float RollValue) { UserRoll = RollValue; };

	void SetThrust(float Thrust) { CurrentThrust = Thrust; };

	EThrottleStage ReturnThrottleStage() { return currentStage; };

	EThrottleStage ReturnPrevThrottleStage() { return prevStage; };
};
