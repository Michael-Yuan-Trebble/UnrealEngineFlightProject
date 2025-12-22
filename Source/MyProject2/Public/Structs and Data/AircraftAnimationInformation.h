// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AircraftAnimationInformation.generated.h"

USTRUCT(BlueprintType)
struct FAircraftAnimationInformation 
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	float MaxSlats;

	UPROPERTY(EditAnywhere)
	float MaxPitchFlaps;

	UPROPERTY(EditAnywhere)
	float MinPitchFlaps;

	UPROPERTY(EditAnywhere)
	float MaxRollFlaps;

	UPROPERTY(EditAnywhere)
	float MaxPitchElevator;

	UPROPERTY(EditAnywhere)
	float MaxRollElevator;

	UPROPERTY(EditAnywhere)
	float MaxRudder;

	UPROPERTY(EditAnywhere)
	float MinExhaust;

	UPROPERTY(EditAnywhere)
	float MaxExhaust;

	UPROPERTY(EditAnywhere)
	float MaxAirbrake;
};