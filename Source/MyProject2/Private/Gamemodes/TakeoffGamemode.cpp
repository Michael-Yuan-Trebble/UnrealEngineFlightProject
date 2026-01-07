// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Takeoff!"));
#include "Gamemodes/TakeoffGamemode.h"
#include "Units/Aircraft/Player/PlayerAircraft.h"
#include "Player Info/AircraftPlayerController.h"
#include "GameFramework/PlayerStart.h"

ATakeoffGamemode::ATakeoffGamemode() {
	FlightMode = EFlightMode::Gears;
	PrimaryActorTick.bCanEverTick = true;
}

void ATakeoffGamemode::BeginPlay() {
	Super::BeginPlay();
}

void ATakeoffGamemode::Tick(float D) {
	Super::Tick(D);
}

void ATakeoffGamemode::CheckTakeoff(float D) {
	if (!IsValid(PlayerSpawn) || !IsValid(PlayerSpawnedIn)) return;
	FVector PlayerLoc = PlayerSpawnedIn->GetActorLocation();
	FVector StartLoc = PlayerSpawn->GetActorLocation();
	float Dist = FVector::Dist(PlayerLoc, StartLoc);
	if (Distance >= Dist) {
		TakeoffTimer -= D;
		if (TakeoffTimer <= 0) {
			FinishMission();
		}
	}
}