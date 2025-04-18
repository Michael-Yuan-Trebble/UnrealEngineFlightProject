// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "FDetectedAircraftInfo.h"
#include "F16AI.generated.h"

UCLASS()
class MYPROJECT2_API AF16AI : public APawn
{
	GENERATED_BODY()

public:

	AF16AI();

	//UObjects and Animation Variables

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USkeletalMeshComponent* AIMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ListedAcceleration = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float TakeoffSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float RollRate = 25.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ListedTurnRate = 0.45f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int ListedMaximumSpeed = 50;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ListedThrust = 1500.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ListedRudder = 0.25f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float InputPitchValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float InputRollValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float InputYawValue;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ReturnElevation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ReturnRollElevation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector LReturnSlatsVector;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector RReturnSlatsVector;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector RudderVector;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector LFlap;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector RFlap;

	FDetectedAircraftInfo ReturnTargeting();

	void RollToTarget(float RollInput, float DeltaSeconds);

protected:

	float currentRollAI;

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	void ScanForTargets();

	void PickTarget();

	TArray<FDetectedAircraftInfo> DetectedTargets;
	
	FDetectedAircraftInfo Targeting;

	float RadarRange = 1000;

	FTimerHandle RadarScanTimer;

private:

	void PitchElevatorCalculation(float DeltaSeconds);

	void RollElevatorCalculation(float DeltaSeconds);

	void SlatsElevationCalculation(float DeltaSeconds);

	void RudderYawCalculation(float DeltaSeconds);

	void FlapPitchCalculation(float DeltaSeconds);
};
