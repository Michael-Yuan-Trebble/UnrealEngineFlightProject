// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "AircraftRegistry.generated.h"

class ABaseUnit;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnAnyUnitHit, AActor* Launcher);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnAnyUnitDeath, AActor* Launcher);

UCLASS()
class MYPROJECT2_API UAircraftRegistry : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UAircraftRegistry();

	FOnAnyUnitHit OnAnyUnitHit;
	FOnAnyUnitDeath OnAnyUnitDeath;

	TArray<TWeakObjectPtr<ABaseUnit>> RegisteredUnits;

	void Register(ABaseUnit* Aircraft);
	void Unregister(ABaseUnit* Aircraft);

	static UAircraftRegistry* Get(UWorld* World);
};
