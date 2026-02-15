// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MissionData.generated.h"

USTRUCT(BlueprintType)
struct FMissionData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FName LevelName = NAME_None;

	UPROPERTY(EditAnywhere)
	bool bNaval;
};
