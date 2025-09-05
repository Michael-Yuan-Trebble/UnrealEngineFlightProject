// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Aircraft/BaseAircraft.h"
#include "Structs and Data/FDetectedAircraftInfo.h"
#include "Aircraft/AI/AIFlightComponent.h"
#include "EnemyAircraft.generated.h"

class AEnemyAircraftAI;
class URadarComponent;

UCLASS()
class MYPROJECT2_API AEnemyAircraft : public ABaseAircraft
{
	GENERATED_BODY()

public:

	AEnemyAircraft();

	UAIFlightComponent* FlightComponent;

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
