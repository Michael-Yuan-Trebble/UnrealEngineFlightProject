// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ThrottleStage.generated.h"

UENUM(BlueprintType)
enum class EThrottleStage : uint8
{
	Slow UMETA(DisplayName = "Slow"),
	Normal UMETA(DisplayName = "Normal"),
	Afterburner UMETA(DisplayName = "Afterburner")
};
