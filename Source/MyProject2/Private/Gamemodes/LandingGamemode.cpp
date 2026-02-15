// Fill out your copyright notice in the Description page of Project Settings.


#include "Gamemodes/LandingGamemode.h"
#include "Units/Aircraft/Player/PlayerAircraft.h"
#include "Player Info/AircraftPlayerController.h"

ALandingGamemode::ALandingGamemode() 
{
	FlightMode = EFlightMode::Gears;
	PrimaryActorTick.bCanEverTick = true;
}

void ALandingGamemode::BeginPlay() {
	Super::BeginPlay();
}

void ALandingGamemode::Tick(float D)
{
	Super::Tick(D);
	CheckLanding(D);
}

void ALandingGamemode::CheckLanding(const float D) 
{
	if (!IsValid(PlayerSpawnedIn)) return;
	if (PlayerSpawnedIn->IsLanded() && PlayerSpawnedIn->GetSpeed() <= 20)
	{
		LandingTimeCounter -= D;
		if (LandingTimeCounter <= 0) 
		{
			FinishMission();
		}
	}
}