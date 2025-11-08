// Fill out your copyright notice in the Description page of Project Settings.


#include "AircraftRegistry.h"
#include "Aircraft/BaseAircraft.h"
#include "EngineUtils.h"

AAircraftRegistry::AAircraftRegistry() 
{
	PrimaryActorTick.bCanEverTick = false;
}

void AAircraftRegistry::Register(ABaseUnit* Aircraft)
{
	if (Aircraft && !RegisteredAircraft.Contains(Aircraft))
	{
		RegisteredAircraft.Add(Aircraft);
	}
}

void AAircraftRegistry::Unregister(ABaseUnit* Aircraft)
{
	RegisteredAircraft.Remove(Aircraft);
}

AAircraftRegistry* AAircraftRegistry::Get(UWorld* World) 
{
	if (!World) return nullptr;

	AAircraftRegistry* Found = nullptr;
	for (TActorIterator<AAircraftRegistry> It(World); It; ++It) 
	{
		if (!Found) Found = *It;
	}
	return Found;
}