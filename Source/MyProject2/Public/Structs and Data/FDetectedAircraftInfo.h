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
	FVector Location = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator Rotation = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	APawn* CurrentPawn = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float threatLevel = 0.f;

	bool operator==(const FDetectedAircraftInfo& Other) const {
		return CurrentPawn == Other.CurrentPawn;
	}

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