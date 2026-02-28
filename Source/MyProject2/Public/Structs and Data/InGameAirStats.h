// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InGameAirStats.generated.h"

USTRUCT(BlueprintType)
struct MYPROJECT2_API FInGameAirStats
{
public:
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	float MaxSpeed = 0.f;

	UPROPERTY(EditAnywhere)
	float Acceleration = 0.f;

	UPROPERTY(EditAnywhere)
	float TurnRate = 0.f;

	UPROPERTY(EditAnywhere)
	float AOARecoverySpeed = 0.f;

	UPROPERTY(EditAnywhere)
	float RudderRate = 0.f;

	UPROPERTY(EditAnywhere)
	float RollRate = 0.f;

	UPROPERTY(EditAnywhere)
	float SpringArmLength = 0.f;

	UPROPERTY(EditAnywhere)
	float NumOfPylons = 0.f;

	UPROPERTY(EditAnywhere)
	float DragCoefficient = 0.f;

	UPROPERTY(EditAnywhere)
	int MinSpeedRangeTurn = 0;

	UPROPERTY(EditAnywhere)
	int MaxSpeedRangeTurn = 0;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UCurveFloat> DragCurve = nullptr;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UCurveFloat> CompressionCurve = nullptr;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMesh> Pylon = nullptr;

};
