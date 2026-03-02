// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Structs and Data/InGameMissileStats.h"
#include "Structs and Data/Weapon Data/BaseWeaponStats.h"
#include "IRMissileStats.generated.h"

UCLASS()
class MYPROJECT2_API UIRMissileStats : public UBaseWeaponStats
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere)
	FInGameMissileStats InGameMissileStats{};
};
