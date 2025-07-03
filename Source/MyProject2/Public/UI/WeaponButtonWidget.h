// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Structs and Data/AircraftData.h"
#include "Blueprint/UserWidget.h"
#include "WeaponButtonWidget.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT2_API UWeaponButtonWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetupWeapons(UAircraftData* AircraftData);
};
