// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AI/BaseSpawnPoint.h"
#include "Structs and Data/MissionWave.h"
#include "BaseMissionController.generated.h"

UCLASS()
class MYPROJECT2_API ABaseMissionController : public AActor
{
	GENERATED_BODY()
	
public:	
	ABaseMissionController();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Mission")
	TArray<ABaseSpawnPoint*> AllSpawnPoints;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Mission")
	TArray<FMissionWave> MissionWaves;

	UFUNCTION(BlueprintCallable)
	void StartMission();

	UFUNCTION(BlueprintCallable)
	void StartWave(int32 WaveIndex);

	UFUNCTION(BlueprintCallable)
	void OnEnemyDestroyed();

private:
	int32 CurrentWave = 0;
	int32 RemainingEnemies = 0;
};
