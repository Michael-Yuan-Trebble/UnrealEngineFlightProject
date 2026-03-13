// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GunAudios.generated.h"

USTRUCT(BlueprintType)
struct FGunAudios
{

	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<USoundWave> GunSound = nullptr;
};
