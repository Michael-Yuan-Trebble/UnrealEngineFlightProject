// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/MissionWaveActor.h"
#include "AI/BaseSpawnPoint.h"
#include "Gamemodes/BaseMissionController.h"
#include "Gamemodes/StandardMissionGamemode.h"
#include "Debug/DebugHelper.h"

AMissionWaveActor::AMissionWaveActor()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AMissionWaveActor::BeginPlay() {
	Super::BeginPlay();
	FTimerHandle MissionTimer;
	GetWorldTimerManager().SetTimer(
		MissionTimer,
		this,
		&AMissionWaveActor::RegisterWave,
		2.f,
		false
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

void AMissionWaveActor::OnWaveDestroyed(const ABaseSpawnPoint* Wave) {
	WaveCount--;
	if (WaveCount <= 0) {
		AIR_DEBUG_KEY(0, FColor::Yellow, "WAVE DESTROYED");
		HandleOnWaveDestroyed.Broadcast(this);
	}
}