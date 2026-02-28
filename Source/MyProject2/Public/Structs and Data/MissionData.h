// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enums/TakeoffType.h"
#include "MissionData.generated.h"

USTRUCT(BlueprintType)
struct FMissionData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UWorld> Level = nullptr;

	UPROPERTY(EditAnywhere)
	ETakeoffType TakeoffType = ETakeoffType::Airborne;
};
