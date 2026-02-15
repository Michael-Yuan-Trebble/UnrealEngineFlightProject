// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Structs and Data/Weapon Data/BaseWeaponStats.h"
#include "ARHMissileStats.generated.h"

UCLASS()
class MYPROJECT2_API UARHMissileStats : public UBaseWeaponStats
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TurnRate = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Acceleration = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxSpeed = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LockOnRange = 0.f;
};
