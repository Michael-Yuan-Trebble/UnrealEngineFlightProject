// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Structs and Data/Aircraft Data/AircraftStats.h"
#include "Units/Aircraft/BaseAircraft.h"
#include "Enums/ThrottleStage.h"
#include "Enums/FlightMode.h"
#include "FlightComponent.generated.h"

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

	void ApplySpeed(const float ThrottlePercentage, const float DeltaSeconds);

	void ApplyRot(const float DletaSeconds);

	void SlowSpeed(const float ThrottlePercentage);

	void NormalSpeed(const float ThrottlePercentage);

	void AfterburnerSpeed(const float ThrottlePercentage);

	void ReturnAOA(const float DeltaSeconds);

	void Setup(ABaseAircraft* InControl, UAircraftStats* InStats);
	
	void AddDropSpeed(const float D);

	void Stall(const float D);

	void Landed(const float D);

	void CheckLanding(const float D);

	UPROPERTY(BlueprintReadOnly)
	int displayG = 1;

	UPROPERTY(BlueprintReadOnly)
	float currentSpeed = 0.f;

// Getters/Setters
public:

	bool IsLanded() const { return bLanded; };

	void SetStallSpeed(const float Speed) { StallSpeed = Speed; };

	float GetSpeed() const { return currentSpeed; };

	float GetThrottle() const { return CurrentThrust; };

	void SetDropSpeed(float Speed) { DropSpeed = Speed; };

	void SetFlightMode(const EFlightMode InMode);

	void SetLanded(bool b) { bLanded = b; };

	EThrottleStage ReturnThrottleStage() const { return currentStage; };

	EThrottleStage ReturnPrevThrottleStage() const { return prevStage; };

	void SetInitialSpeed(const float Speed)
	{
		currentSpeed = Speed;
		if (IsValid(Controlled)) Velocity = Controlled->GetActorForwardVector() * Speed;
	}

	float GetCurrentSpeedKMH() const { return FMath::Max((currentSpeed * 0.036f), 0.f); };

	float ConvertKMHToSpeed(const float Speed) const { return FMath::Max((Speed / 0.036f), 0.f); };

	EThrottleStage getThrottleStage(const float throttle);

	void SetPitch(const float PitchValue) { UserRotation.Pitch = PitchValue; };

	void SetYaw(const float YawValue) { UserRotation.Yaw = YawValue; };

	void SetRoll(const float RollValue) { UserRotation.Roll = RollValue; };

	void SetThrust(const float Thrust) { CurrentThrust = Thrust; };

	const FRotator& GetUserRotation() const { return UserRotation; };

	const FRotator& GetNextRotation() const { return NextRotation; };

	float GetThrust() const { return CurrentThrust; };

	void SetFlying(const bool InFly) { isFlying = InFly; bLanded = false; };

	void AddSpeed(const float Speed, const float D);

private:
	float FlightDrag = 0.f;
	
	FQuat PrevQuat = FQuat::Identity;

	float DropSpeed = 0.f;

	float StallSpeed = 0.f;

	FRotator UserRotation = FRotator::ZeroRotator;

	FRotator NextRotation = FRotator::ZeroRotator;

	float CurrentThrust = 0.f;

	float targetSpeed = 0.f;

	float Acceleration = 0.f;

	float DownPitch = 0.f;

	bool bLanded = false;

	bool bCanTakeOff = false;

	float MaxPitch = 0.f;

	bool bDropping = false;

	FVector Velocity = FVector::ZeroVector;

	FVector PreviousVelocity = FVector::ZeroVector;

	float previousGForce = 0.f;

	EThrottleStage prevStage = EThrottleStage::Slow;

	EThrottleStage currentStage = EThrottleStage::Normal;

	EFlightMode FlightMode = EFlightMode::Flight;

	bool switchingPhase = false;

	UPROPERTY()
	UAircraftStats* AircraftStats = nullptr;

	UPROPERTY()
	ABaseAircraft* Controlled = nullptr;

	bool isFlying = false;

	static constexpr float VortexG = 9.f;
	static constexpr float TakeoffSpeed = 10.f;
	static constexpr int Interp = 50;

private:

	float CalculateSpeedDrag();

	void CalculateGForce(const float DeltaSeconds);

	float DragAOA();

	void RollAOA(const float DeltaSeconds);

	float GetAOA();

	float PitchDrag();

	void ApplyPitch(const float DeltaSeconds);

	void ApplyYaw(const float DeltaSeconds);

	void ApplyRoll(const float DeltaSeconds);

	void TempRecovery(const float D, const float Deg);

	void CalculateVortex();

	FORCEINLINE bool IsControlledValid() const {
		return !IsValid(Controlled);
	}

	FORCEINLINE bool IsAirframeValid() const {
		return !IsValid(Controlled) || !IsValid(Controlled->Airframe);
	}
};
