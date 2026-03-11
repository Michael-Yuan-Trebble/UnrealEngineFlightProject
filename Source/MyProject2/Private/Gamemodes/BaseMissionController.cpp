// Fill out your copyright notice in the Description page of Project Settings.

#include "Gamemodes/BaseMissionController.h"
#include "Kismet/GameplayStatics.h"
#include "AI/MissionWaveActor.h"
#include "Gamemodes/StandardMissionGamemode.h"
#include "Debug/DebugHelper.h"

ABaseMissionController::ABaseMissionController() {

}

void ABaseMissionController::Setup(AStandardMissionGamemode* InMission) {
	CurrentMission = InMission;
}

void ABaseMissionController::BeginPlay() {
	Super::BeginPlay();
}

void ABaseMissionController::RegisterWave(AMissionWaveActor* InSpawn) {
	if (!IsValid(InSpawn)) return;
	InSpawn->HandleOnWaveDestroyed.AddDynamic(this, &ABaseMissionController::OnEnemyDestroyed);
	MissionWaves.FindOrAdd(InSpawn->WaveIndex).Add(InSpawn);
	if (InSpawn->WaveIndex > TotalWaves) TotalWaves = InSpawn->WaveIndex;
}

void ABaseMissionController::StartMission() {

}

void ABaseMissionController::StartWave(const int32 WaveIndex) {
	CurrentWave = WaveIndex;
	if (auto* Waves = MissionWaves.Find(WaveIndex)) {
		for (AMissionWaveActor* Wave : *Waves) {
			if (IsValid(Wave)) {
				Wave->ActivateSpawnPoints();
			}
		}
	}
}

void ABaseMissionController::OnEnemyDestroyed(const AMissionWaveActor* InWave) {
	int32 WaveIndex = InWave->WaveIndex;
	CompletedWaves.FindOrAdd(WaveIndex)++;
	if (auto* Waves = MissionWaves.Find(WaveIndex)) {
		if (CompletedWaves[WaveIndex] >= Waves->Num()) {
			HandleWaveCompleted(WaveIndex);
		}
	}
}

void ABaseMissionController::HandleWaveCompleted(int32 WaveIndex) {
	AIR_DEBUG_KEY(1, FColor::Red, "Wave %d Destroyed", WaveIndex);
	if (WaveIndex >= TotalWaves) MissionComplete();
	else StartWave(WaveIndex + 1);
}

void ABaseMissionController::MissionComplete() {
	if (IsValid(CurrentMission)) CurrentMission->MissionFinish();
}