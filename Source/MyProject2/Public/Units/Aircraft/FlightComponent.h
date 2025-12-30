// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Structs and Data/Aircraft Data/AircraftStats.h"
#include "Units/Aircraft/BaseAircraft.h"
#include "Enums/ThrottleStage.h"
#include "Enums/FlightMode.h"
#include "FlightComponent.generated.h"

#define VORTEXG 9.f
#define TAKEOFFSPEED 10.f

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAfterburnerEngaged, bool, isActive);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnVortexActivate, bool, isVortex);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MYPROJECT2_API UFlightComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	FOnAfterburnerEngaged OnAfterburnerEngaged;

	FOnVortexActivate OnVortexActivate;

	UFlightComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY()
	UAircraftStats* AircraftStats;

	UPROPERTY()
	ABaseAircraft* Controlled;

	bool isFlying = false;

	UPROPERTY(BlueprintReadOnly)
	float currentSpeed = 0.f;

	float UserPitch = 0.f;

	float UserYaw = 0.f;

	float UserRoll = 0.f;

	float NextPitch = 0.f;

	float NextYaw = 0.f;

	float NextRoll = 0.f;

	float CurrentThrust = 0.f;

	float targetSpeed = 0.f;

	float Acceleration = 0.f;
	
	float DownPitch = 0.f;

	UPROPERTY(BlueprintReadOnly)
	int displayG = 1;

	FVector Velocity = FVector::ZeroVector;

	FVector PreviousVelocity = FVector::ZeroVector;

	float previousGForce = 0.f;

	EThrottleStage prevStage = EThrottleStage::Slow;

	EThrottleStage currentStage = EThrottleStage::Normal;

	EFlightMode FlightMode = EFlightMode::Flight;

	bool switchingPhase = false;

	void ApplySpeed(float ThrottlePercentage, float DeltaSeconds);

	void ApplyRot(float DletaSeconds);

	void SlowSpeed(float ThrottlePercentage);

	void NormalSpeed(float ThrottlePercentage);

	void AfterburnerSpeed(float ThrottlePercentage);

	void ReturnAOA(float DeltaSeconds);

	void Setup(ABaseAircraft* InControl, UAircraftStats* InStats);

	EThrottleStage getThrottleStage(float throttle);

	void SetPitch(float PitchValue) { UserPitch = PitchValue; };

	void SetYaw(float YawValue) { UserYaw = YawValue; };

	void SetRoll(float RollValue) { UserRoll = RollValue; };

	void SetThrust(float Thrust) { CurrentThrust = Thrust; };

	float GetRoll() { return NextRoll; };

	void SetFlightMode(EFlightMode InMode);

	EThrottleStage ReturnThrottleStage() const { return currentStage; };

	EThrottleStage ReturnPrevThrottleStage() const { return prevStage; };

	float GetSpeed() const { return currentSpeed; };

	float GetThrottle() const { return CurrentThrust; };

private:
	float CalculateSpeedDrag();

	void CalculateGForce(float DeltaSeconds);

	float DragAOA();

	void RollAOA(float DeltaSeconds);

	float GetAOA();

	float PitchDrag();

	void ApplyPitch(float DeltaSeconds);

	void ApplyYaw(float DeltaSeconds);

	void ApplyRoll(float DeltaSeconds);

	void CalculateVortex();

	float GetCurrentSpeedKMH() const { return FMath::Max((currentSpeed * 0.036f),0); };
};
