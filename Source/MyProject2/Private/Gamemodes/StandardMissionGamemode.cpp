// Fill out your copyright notice in the Description page of Project Settings.

#include "Gamemodes/StandardMissionGamemode.h"
#include "Kismet/GameplayStatics.h"
#include "Gamemodes/BaseMissionController.h"
#include "AI/BaseSpawnPoint.h"

void AStandardMissionGamemode::BeginPlay() {
	Super::BeginPlay();

	if (IsValid(MissionControllerClass) && IsValid(GetWorld())) {
		MissionController = GetWorld()->SpawnActor<ABaseMissionController>(MissionControllerClass);
		if (MissionController) {
			// Init Function
		}
	}
}

void AStandardMissionGamemode::RegisterSpawnPoint(ABaseSpawnPoint* InSpawn) {
	if (IsValid(MissionController)) {
		MissionController->RegisterSpawnPoint(InSpawn);
	}
}

void AStandardMissionGamemode::Tick(float D) {
	Super::Tick(D);
}