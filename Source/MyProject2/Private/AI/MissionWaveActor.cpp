// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/MissionWaveActor.h"
#include "AI/BaseSpawnPoint.h"
#include "Gamemodes/BaseMissionController.h"
#include "Gamemodes/StandardMissionGamemode.h"
#include "Debug/DebugHelper.h"

AMissionWaveActor::AMissionWaveActor() {
	PrimaryActorTick.bCanEverTick = true;
}

void AMissionWaveActor::BeginPlay() {
	Super::BeginPlay();
	FTimerHandle MissionTimer;
	GetWorldTimerManager().SetTimer(
		MissionTimer, this,
		&AMissionWaveActor::RegisterWave,
		0.05f, false
	);
}

void AMissionWaveActor::RegisterWave() {
	AStandardMissionGamemode* GM = GetWorld()->GetAuthGameMode<AStandardMissionGamemode>();
	if (IsValid(GM) && GM->GetMissionController())
		GM->GetMissionController()->RegisterWave(this);
}

void AMissionWaveActor::ActivateSpawnPoints() {
	for (ABaseSpawnPoint* Spawn : WaveData.SpawnPoints) {
		if (!IsValid(Spawn)) continue;
		if (Spawn->bStressTest) Spawn->StressTest();
		else Spawn->ActivateSpawn();
		Spawn->OnWaveDestroyed.AddDynamic(this, &AMissionWaveActor::OnWaveDestroyed);
		WaveCount++;
	}
}

void AMissionWaveActor::OnWaveDestroyed(ABaseSpawnPoint* Wave) {
	WaveCount--;
	if (WaveCount <= 0) {
		RemoveAllDynamics();
		HandleOnWaveDestroyed.Broadcast(this);
	}
}

void AMissionWaveActor::RemoveAllDynamics() {
	for (ABaseSpawnPoint* Spawn : WaveData.SpawnPoints) {
		if (!IsValid(Spawn)) continue;
		Spawn->OnWaveDestroyed.RemoveAll(this);
	}
}

void AMissionWaveActor::EndPlay(EEndPlayReason::Type EndPlay) {
	RemoveAllDynamics();
	Super::EndPlay(EndPlay);
}