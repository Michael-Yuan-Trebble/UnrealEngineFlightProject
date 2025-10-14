// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Structs and Data/FDetectedAircraftInfo.h"
#include "UI/LockBoxWidget.h"
#include "Components/WidgetComponent.h"
#include "Aircraft/AI/EnemyAircraft.h"
#include "F16AI.generated.h"

UCLASS()
class MYPROJECT2_API AF16AI : public AEnemyAircraft
{
	GENERATED_BODY()

public:

	AF16AI();

	//UObjects and Animation Variables

	FDetectedAircraftInfo* ReturnTargeting();

	void ApplySpeed(float ThrottlePercentage);

	void RollToTarget(float RollInput, float DeltaSeconds);

	void PitchToTarget(float PitchInput, float DeltaSeconds);

	float YawBoxWidth = 100.f;

	float YawBoxHeight = 100.f;

	float YawBoxDepth = 100.f;

	FDetectedAircraftInfo* Targeting;

protected:

	float currentRollAI;

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	float RadarRange = 1000;

	FTimerHandle RadarScanTimer;

private:

	void PitchElevatorCalculation(float DeltaSeconds);

	void RollElevatorCalculation(float DeltaSeconds);

	void SlatsElevationCalculation(float DeltaSeconds);

	void RudderYawCalculation(float DeltaSeconds);

	void FlapPitchCalculation(float DeltaSeconds);
};
