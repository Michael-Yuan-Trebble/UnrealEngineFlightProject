// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "TargetTypes.generated.h"

UENUM(BlueprintType)
enum class ETargetType : uint8 
{
	Air UMETA(DisplayName = "Air"),
	Ground UMETA(DisplayName = "Ground"),
	Naval UMETA(DisplayName = "Naval")
};