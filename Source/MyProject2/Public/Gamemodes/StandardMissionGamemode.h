// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gamemodes/FlightGamemode.h"
#include "StandardMissionGamemode.generated.h"

class ABaseMissionController;
class ABaseSpawnPoint;

UCLASS()
class MYPROJECT2_API AStandardMissionGamemode : public AFlightGamemode
{
	GENERATED_BODY()
	
public:

	ABaseMissionController* GetMissionController() const { return MissionController; };

	void RegisterSpawnPoint(ABaseSpawnPoint* InSpawn);

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY()
	TObjectPtr<ABaseMissionController> MissionController = nullptr;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ABaseMissionController> MissionControllerClass = nullptr;
};
