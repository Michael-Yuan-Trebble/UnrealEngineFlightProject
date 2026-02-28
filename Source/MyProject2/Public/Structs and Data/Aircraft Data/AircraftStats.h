// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Structs and Data/Weapon Data/BulletStats.h"
#include "Structs and Data/InGameAirStats.h"
#include "AircraftStats.generated.h"

UCLASS()
class MYPROJECT2_API UAircraftStats : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName AircraftName = NAME_None;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float MaxSpeed = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Acceleration = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TurnRate = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AOARecoverySpeed = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RudderRate = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RollRate = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SpringArmLength = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int NumOfPylons = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UStaticMesh> Pylon = nullptr;

	// TODO: Flesh this out, plan is to have two types of planes, one-circle and two-circle performers aka rate vs instant turn
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxAOA = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DragCoefficient = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int MinSpeedRangeTurn = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int MaxSpeedRangeTurn = 0;

	// TODO: Might just make this one curve and call it like turn curve or something

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UCurveFloat> DragCurve = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UCurveFloat> CompressionCurve = nullptr;

	UPROPERTY(EditAnywhere)
	FInGameAirStats InGameAirStats{};
};
