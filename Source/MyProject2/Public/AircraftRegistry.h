// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "AircraftRegistry.generated.h"

class ABaseUnit;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnAnyUnitHit, AActor* Launcher);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnAnyUnitDeath, AActor* Launcher);

UCLASS()
class MYPROJECT2_API AAircraftRegistry : public AInfo
{
	GENERATED_BODY()

public:
	AAircraftRegistry();

	FOnAnyUnitHit OnAnyUnitHit;
	FOnAnyUnitDeath OnAnyUnitDeath;

	UPROPERTY(BlueprintReadOnly)
	TArray<ABaseUnit*> RegisteredAircraft;

	void Register(ABaseUnit* Aircraft);
	void Unregister(ABaseUnit* Aircraft);

	static AAircraftRegistry* Get(UWorld* World);
};
