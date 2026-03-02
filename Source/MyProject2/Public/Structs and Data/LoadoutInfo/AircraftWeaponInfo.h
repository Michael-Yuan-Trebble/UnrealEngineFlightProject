// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AircraftWeaponInfo.generated.h"

USTRUCT(BlueprintType)
struct MYPROJECT2_API FAircraftWeaponInfo
{
public:
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMesh> Pylon = nullptr;

	UPROPERTY(EditAnywhere)
	float NumOfPylons = 0.f;
};
