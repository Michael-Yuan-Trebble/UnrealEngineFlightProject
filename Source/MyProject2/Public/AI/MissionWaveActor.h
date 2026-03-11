// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Structs and Data/MissionInfo/MissionWave.h"
#include "MissionWaveActor.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWaveDestroyed, const AMissionWaveActor*, Wave);

class ABaseSpawnPoint;

UCLASS()
class MYPROJECT2_API AMissionWaveActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AMissionWaveActor();

	FOnWaveDestroyed HandleOnWaveDestroyed;

	UPROPERTY(EditInstanceOnly)
	FMissionWave WaveData{};

	UPROPERTY(EditAnywhere)
	int32 WaveIndex = 0;

	UPROPERTY()
	int32 WaveCount = 0;

	void ActivateSpawnPoints();

protected:

	virtual void BeginPlay() override;

	void RegisterWave();

	UFUNCTION()
	void OnWaveDestroyed(const ABaseSpawnPoint* Wave);

};
