// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Structs and Data/Weapon Data/BaseWeaponStats.h"
#include "BombStats.generated.h"

UCLASS()
class MYPROJECT2_API UBombStats : public UBaseWeaponStats
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BlastRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxSpeed;
};
