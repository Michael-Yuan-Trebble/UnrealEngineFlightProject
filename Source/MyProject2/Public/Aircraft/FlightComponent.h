// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Structs and Data/Aircraft Data/AircraftStats.h"
#include "Aircraft/BaseAircraft.h"
#include "FlightComponent.generated.h"

UENUM(BlueprintType)
enum class EThrottleStage : uint8
{
	Slow UMETA(DisplayName = "Slow"),
	Normal UMETA(DisplayName = "Normal"),
	Afterburner UMETA(DisplayName = "Afterburner")
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYPROJECT2_API UFlightComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
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

	bool switchingPhase = false;

	void ApplySpeed(float ThrottlePercentage, float DeltaSeconds);

	void ApplyRot(float DletaSeconds);

	void SetPitch(float PitchValue);

	void SetYaw(float YawValue);

	void SetRoll(float RollValue);

	void SetThrust(float Thrust);

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
};
