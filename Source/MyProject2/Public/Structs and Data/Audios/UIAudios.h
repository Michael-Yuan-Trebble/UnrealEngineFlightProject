// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UIAudios.generated.h"

USTRUCT(BlueprintType)
struct FUIAudios
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<USoundWave> LockingSound = nullptr;

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<USoundWave> LockedSound = nullptr;
};
