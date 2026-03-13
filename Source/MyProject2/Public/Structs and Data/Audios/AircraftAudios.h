// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AircraftAudios.generated.h"

USTRUCT(BlueprintType)
struct FAircraftAudios
{

	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<USoundWave> CockpitSound = nullptr;

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<USoundWave> ThirdPersonSound = nullptr;
};
