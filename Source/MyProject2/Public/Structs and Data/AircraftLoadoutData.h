// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AircraftLoadoutData.generated.h"

class ABaseWeapon;
class ABaseAircraft;
class UBaseSpecial;

USTRUCT(BlueprintType)
struct FAircraftLoadoutData
{
	GENERATED_BODY()

	UPROPERTY()
	TSubclassOf<ABaseAircraft> AircraftClass;

	UPROPERTY()
	TMap<FName, TSubclassOf<ABaseWeapon>> EquippedWeapons;

	UPROPERTY()
	TSubclassOf<UBaseSpecial> SelectedSpecial;
};
