// Fill out your copyright notice in the Description page of Project Settings.


#include "AircraftRegistry.h"
#include "Aircraft/BaseAircraft.h"
#include "EngineUtils.h"

UAircraftRegistry::UAircraftRegistry() {}

void UAircraftRegistry::Register(ABaseUnit* Aircraft)
{
	if (Aircraft && !RegisteredUnits.Contains(Aircraft))
	{
		RegisteredUnits.Add(Aircraft);
	}
}

void UAircraftRegistry::Unregister(ABaseUnit* Aircraft)
{
	RegisteredUnits.Remove(Aircraft);
}

UAircraftRegistry* UAircraftRegistry::Get(UWorld* World)
{
	if (!World) return nullptr;

	UGameInstance* GI = World->GetGameInstance();
	if (!GI) return nullptr;

	return GI->GetSubsystem<UAircraftRegistry>();
}