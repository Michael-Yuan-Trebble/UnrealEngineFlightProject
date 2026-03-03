// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MissionWave.generated.h"

USTRUCT(BlueprintType)
struct FMissionWave
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TArray<class ABaseSpawnPoint*> SpawnPoints{};

	UPROPERTY(EditAnywhere)
	FString ObjectiveName = TEXT("");

	UPROPERTY(EditAnywhere)
	bool bWaitForCompletion = true;
};