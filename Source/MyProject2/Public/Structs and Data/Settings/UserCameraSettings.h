// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UserCameraSettings.generated.h"

USTRUCT(BlueprintType)
struct FUserCameraSettings
{

	GENERATED_BODY()

	// For now just the sensitivity, might put the roll lag strength and stuff if I think it's something that would be better tweaked
	UPROPERTY(EditAnywhere)
	float Sensitivity = 1.f;
};
