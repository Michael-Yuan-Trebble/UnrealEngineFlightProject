// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EControlMode : uint8
{
	Aircraft UMETA(DisplayName = "Aircraft"),
	Menu UMETA(DisplayName = "Menu"),
	Null UMETA(DisplayName = "NULL")
};