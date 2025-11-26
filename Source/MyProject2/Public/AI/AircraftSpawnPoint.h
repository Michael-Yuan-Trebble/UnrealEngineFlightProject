// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/BaseSpawnPoint.h"
#include "AircraftSpawnPoint.generated.h"

UCLASS()
class MYPROJECT2_API AAircraftSpawnPoint : public ABaseSpawnPoint
{
	GENERATED_BODY()
	
public:

	// TEMPORARY, just to test things easier spawn points should be triggered by gamemode not just automatically on its own
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	TSubclassOf<APawn> AircraftClass;

	UPROPERTY(EditAnywhere)
	int32 Count = 1;

	// TODO: Do formations later, for now they just spawn in a line
	UPROPERTY(EditAnywhere)
	float FormationSpacing = 600.f;

	virtual void ActivateSpawn() override;
};
