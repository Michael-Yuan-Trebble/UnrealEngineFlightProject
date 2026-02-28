// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InGameBombStats.generated.h"

USTRUCT(BlueprintType)
struct MYPROJECT2_API FInGameBombStats
{
public:

	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	float BlastRadius = 0.f;

	UPROPERTY(EditAnywhere)
	float MaxSpeed = 0.f;
};
