// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Structs and Data/MissionInfo/MissionWave.h"
#include "AI/MissionWaveActor.h"
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

	void RegisterWave(AMissionWaveActor* InSpawn);

protected:
	virtual void BeginPlay() override;

private:

	TMap<int32, TArray<AMissionWaveActor*>> MissionWaves{};

	UPROPERTY()
	TObjectPtr<AStandardMissionGamemode> CurrentMission = nullptr;

	TMap<int32, int32> CompletedWaves{};

	UFUNCTION()
	void OnEnemyDestroyed(const AMissionWaveActor* InMission);

	void HandleWaveCompleted(int32 WaveIndex);

	void MissionComplete();

	int32 CurrentWave = 0;
	int32 TotalWaves = 0;

};
