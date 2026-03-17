// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Structs and Data/LoadoutInfo/PylonLoadout.h"
#include "AircraftWeaponEquipInfo.generated.h"

USTRUCT(BlueprintType)
struct FAircraftWeaponEquipInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TArray<FName> Pylons;

	UPROPERTY(EditAnywhere)
	FPylonLoadout PylonLoadout;
};
