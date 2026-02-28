// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/BaseSpawnPoint.h"
#include "MissionWave.generated.h"

USTRUCT(BlueprintType)
struct FMissionWave
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TArray<ABaseSpawnPoint*> SpawnPoints{};

	UPROPERTY(EditAnywhere)
	FString ObjectiveName = TEXT("");

	UPROPERTY(EditAnywhere)
	bool bWaitForCompletion = true;
};