// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Structs and Data/AircraftData.h"
#include "WeaponSelectionWidget.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT2_API UWeaponSelectionWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UWeaponSelectionWidget(const FObjectInitializer& ObjectInitializer);
};
