// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BaseSpawnPoint.h"
#include "Gamemodes/StandardMissionGamemode.h"
#include "Gamemodes/BaseMissionController.h"

void ABaseSpawnPoint::ActivateSpawn() {
}

void ABaseSpawnPoint::OnUnitDestroyed(AActor* DestroyedActor) {
	AliveUnits.Remove(DestroyedActor);
	if (AliveUnits.Num() == 0) {
		OnWaveDestroyed.Broadcast(this);
	}
}