// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Aircraft/BaseAircraft.h"
#include "Structs and Data/FDetectedAircraftInfo.h"
#include "Aircraft/AI/AIFlightComponent.h"
#include "Structs and Data/Aircraft Data/AIAircraftData.h"
#include "EnemyAircraft.generated.h"

class AEnemyAircraftAI;
class URadarComponent;
class UFlightComponent;

UCLASS()
class MYPROJECT2_API AEnemyAircraft : public ABaseAircraft
{
	GENERATED_BODY()

public:

	AEnemyAircraft();

	UFlightComponent* FlightComponent;

	URadarComponent* Radar;

	AEnemyAircraftAI* Controller;

	virtual void PossessedBy(AController* Controller) override;

	void PickTarget();

protected:

	TArray<FDetectedAircraftInfo> DetectedTargets;

	FDetectedAircraftInfo Targeting;

	float RadarRange = 1000;

	FTimerHandle RadarScanTimer;
};
