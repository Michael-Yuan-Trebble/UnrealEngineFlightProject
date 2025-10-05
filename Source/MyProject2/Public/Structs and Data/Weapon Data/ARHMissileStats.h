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
	float TurnRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Acceleration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LockOnRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName InGameName;
};
