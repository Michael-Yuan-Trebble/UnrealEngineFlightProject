// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AircraftAnimationInformation.generated.h"

USTRUCT(BlueprintType)
struct FAircraftAnimationInformation 
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	float MaxSlats = 0.f;

	UPROPERTY(EditAnywhere)
	float MaxPitchFlaps = 0.f;

	UPROPERTY(EditAnywhere)
	float MinPitchFlaps = 0.f;

	UPROPERTY(EditAnywhere)
	float MaxRollFlaps = 0.f;

	UPROPERTY(EditAnywhere)
	float MaxPitchElevator = 0.f;

	UPROPERTY(EditAnywhere)
	float MaxRollElevator = 0.f;

	UPROPERTY(EditAnywhere)
	float MaxRudder = 0.f;

	UPROPERTY(EditAnywhere)
	float MinExhaust = 0.f;

	UPROPERTY(EditAnywhere)
	float MaxExhaust = 0.f;

	UPROPERTY(EditAnywhere)
	float MaxAirbrake = 0.f;
};