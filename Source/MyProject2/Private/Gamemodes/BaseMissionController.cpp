// Fill out your copyright notice in the Description page of Project Settings.

#include "Gamemodes/BaseMissionController.h"
#include "Kismet/GameplayStatics.h"
#include "AI/BaseSpawnPoint.h"
#include "Gamemodes/StandardMissionGamemode.h"

ABaseMissionController::ABaseMissionController() {

}

void ABaseMissionController::Setup(AStandardMissionGamemode* InMission) {
	CurrentMission = InMission;
}

void ABaseMissionController::BeginPlay() {

}

void ABaseMissionController::RegisterSpawnPoint(ABaseSpawnPoint* InSpawn) {

}

void ABaseMissionController::StartMission() {

}

void ABaseMissionController::StartWave(const int32 WaveIndex) {

}

void ABaseMissionController::OnEnemyDestroyed() {

}