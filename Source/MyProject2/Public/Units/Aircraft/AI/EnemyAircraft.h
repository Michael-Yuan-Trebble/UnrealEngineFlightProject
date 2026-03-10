// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Units/Aircraft/BaseAircraft.h"
#include "Structs and Data/InGame/FDetectedAircraftInfo.h"
#include "Structs and Data/Aircraft Data/AIAircraftData.h"
#include "EnemyAircraft.generated.h"

class AEnemyAircraftAI;
class ABaseWeapon;
class UAIFlightComponent;
class AAircraftAIController;

UCLASS()
class MYPROJECT2_API AEnemyAircraft : public ABaseAircraft
{
	GENERATED_BODY()

public:

	AEnemyAircraft();

	virtual void PossessedBy(AController* Controller) override;

	FTimerHandle RepeatTimerHandle;

	virtual void FireBullets() override;
	virtual void StartBullets() override;
	virtual void EndBullets() override;

private:
	TObjectPtr<AAircraftAIController> Controller = nullptr;
};
