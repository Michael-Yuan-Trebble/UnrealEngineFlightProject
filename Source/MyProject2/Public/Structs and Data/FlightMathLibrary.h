// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FlightMathLibrary.generated.h"

UCLASS()
class MYPROJECT2_API UFlightMathLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintPure, Category = "Flight Math")
	static float ConvertKMHToSpeed(float KMH) {
		return FMath::Max((KMH / 0.036f), 0.f);
	};

	UFUNCTION(BlueprintPure, Category = "Flight Math")
	static float SpeedToKMH(float Speed) {
		return FMath::Max((Speed * 0.036f), 0.f);
	}
};
