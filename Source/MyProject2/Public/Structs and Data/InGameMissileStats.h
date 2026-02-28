// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InGameMissileStats.generated.h"

USTRUCT(BlueprintType)
struct MYPROJECT2_API FInGameMissileStats
{
public:
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	float TurnRate = 0.f;

	UPROPERTY(EditAnywhere)
	float Acceleration = 0.f;

	UPROPERTY(EditAnywhere)
	float MaxSpeed = 0.f;

	UPROPERTY(EditAnywhere)
	float LockOnRange = 0.f;
};
