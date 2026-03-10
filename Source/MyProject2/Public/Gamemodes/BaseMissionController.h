// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Structs and Data/MissionInfo/MissionWave.h"
#include "BaseMissionController.generated.h"

class ABaseSpawnPoint;
class AStandardMissionGamemode;

UCLASS()
class MYPROJECT2_API ABaseMissionController : public AActor
{
	GENERATED_BODY()
	
public:	
	ABaseMissionController();

	UFUNCTION(BlueprintCallable)
	void StartMission();

	UFUNCTION(BlueprintCallable)
	void StartWave(const int32 WaveIndex);

	void Setup(AStandardMissionGamemode* InMission);

	void RegisterSpawnPoint(ABaseSpawnPoint* InSpawn);

protected:
	virtual void BeginPlay() override;

private:

	UPROPERTY(EditAnywhere, Category = "Mission")
	TArray<TObjectPtr<ABaseSpawnPoint>> AllSpawnPoints{};

	UPROPERTY(EditAnywhere, Category = "Mission")
	TArray<FMissionWave> MissionWaves{};

	UPROPERTY()
	TObjectPtr<AStandardMissionGamemode> CurrentMission = nullptr;

	UFUNCTION()
	void OnEnemyDestroyed();

	int32 CurrentWave = 0;
	int32 RemainingEnemies = 0;
};
