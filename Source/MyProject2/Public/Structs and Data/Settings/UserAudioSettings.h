// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UserAudioSettings.generated.h"

USTRUCT(BlueprintType)
struct FUserAudioSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	float Volume = 1.f;
};
