// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/BaseSpawnPoint.h"
#include "AircraftSpawnPoint.generated.h"

UCLASS()
class MYPROJECT2_API AAircraftSpawnPoint : public ABaseSpawnPoint
{
	GENERATED_BODY()
	
private:

	// TEMPORARY, just to test things easier spawn points should be triggered by gamemode not just automatically on its own
	virtual void BeginPlay() override;

	virtual void ActivateSpawn() override;

	virtual void StressTest() override;

	void SetInitialSpeed(APawn* Spawn);

	// TODO: Do formations later, for now they just spawn in a line
	UPROPERTY(EditAnywhere)
	float FormationSpacing = 600.f;

	UPROPERTY(EditAnywhere)
	float DestroyDelay = 10.f;

	UPROPERTY(EditAnywhere)
	float InitialSpeed = 0.f;
};
