// Fill out your copyright notice in the Description page of Project Settings.

#include "Gamemodes/StandardMissionGamemode.h"
#include "Kismet/GameplayStatics.h"
#include "Gamemodes/BaseMissionController.h"
#include "AI/BaseSpawnPoint.h"
#include "Debug/DebugHelper.h"

void AStandardMissionGamemode::BeginPlay() {
	Super::BeginPlay();

	if (IsValid(MissionControllerClass) && IsValid(GetWorld())) {
		MissionController = GetWorld()->SpawnActor<ABaseMissionController>(MissionControllerClass);
		if (IsValid(MissionController)) {
			FTimerHandle WaveStartTimer;
			GetWorldTimerManager().SetTimer(
				WaveStartTimer,
				this,
				&AStandardMissionGamemode::StartFirstWave,
				3.f,
				false
			);
		}
	}
}

void AStandardMissionGamemode::StartFirstWave() {
	if (IsValid(MissionController)) MissionController->StartWave(0);
}

void AStandardMissionGamemode::Tick(float D) {
	Super::Tick(D);
}

void AStandardMissionGamemode::MissionFinish() {

}