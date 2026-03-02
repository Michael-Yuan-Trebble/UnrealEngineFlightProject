// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AircraftAnimationValues.generated.h"

USTRUCT(BlueprintType)
struct MYPROJECT2_API FAircraftAnimationValues
{
public:

	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	float Rudder = 0.f;

	UPROPERTY(BlueprintReadOnly)
	float Slat = 0.f;

	UPROPERTY(BlueprintReadOnly)
	float LFlap = 0.f;

	UPROPERTY(BlueprintReadOnly)
	float RFlap = 0.f;

	UPROPERTY(BlueprintReadOnly)
	float Nozzle = 0.f;

	UPROPERTY(BlueprintReadOnly)
	float AirBrake = 0.f;

	UPROPERTY(BlueprintReadOnly)
	float Elevator = 0.f;
};
