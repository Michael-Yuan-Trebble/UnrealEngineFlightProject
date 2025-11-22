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
	UPROPERTY(EditAnywhere)
	TSubclassOf<APawn> AircraftClass;

	UPROPERTY(EditAnywhere)
	int32 Count = 1;

	// TODO: Do formations later, for now they just spawn in a line
	UPROPERTY(EditAnywhere)
	float FormationSpacing = 600.f;

	virtual void ActivateSpawn() override;
};
