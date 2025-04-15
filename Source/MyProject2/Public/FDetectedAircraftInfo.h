// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FDetectedAircraftInfo.generated.h"

USTRUCT(BlueprintType)
struct MYPROJECT2_API FDetectedAircraftInfo
{

public:

	GENERATED_BODY()

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FVector Location;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Distance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator Rotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	APawn* CurrentPawn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float threatLevel;

	FDetectedAircraftInfo();

	/*
		Threat 1.f is Max
		Player has differing threat multiplications
		Example:
			Easy = 0.5f
			Normal = 1.f
			Difficult = 1.5f

	*/
	float CalculateThreat() const {
		return 1.f;
	}

};