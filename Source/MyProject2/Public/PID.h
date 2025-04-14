// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PID.generated.h"

UCLASS()
class MYPROJECT2_API UPID : public UObject
{

	GENERATED_BODY()

public:
	UPID();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Kp; //Proportional Gain

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Ki; //Integral Gain

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Kd; //Derivative Gain

	float previousError;

	float Integral;

	void Initialize(float InKp, float InKi, float InKd);

	float Update(float Setpoint, float CurrentValue, float DeltaTime);

	void Reset();
};
