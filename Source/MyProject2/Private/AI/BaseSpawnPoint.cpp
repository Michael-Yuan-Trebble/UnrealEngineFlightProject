// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BaseSpawnPoint.h"
#include "Gamemodes/StandardMissionGamemode.h"
#include "Gamemodes/BaseMissionController.h"

void ABaseSpawnPoint::ActivateSpawn() {
}

void ABaseSpawnPoint::StressTest() {

}

void ABaseSpawnPoint::OnUnitDestroyed(AActor* DestroyedActor) {
	DestroyedActor->OnDestroyed.RemoveAll(this);
	AliveUnits.Remove(DestroyedActor);
	if (AliveUnits.Num() == 0)
		OnWaveDestroyed.Broadcast(this);
}

void ABaseSpawnPoint::RemoveAllDynamics() {
	for (TWeakObjectPtr<AActor> Unit : AliveUnits) {
		if (AActor* Loaded = Unit.Get())
			Loaded->OnDestroyed.RemoveAll(this);
	}
}

void ABaseSpawnPoint::EndPlay(EEndPlayReason::Type EndPlay) {
	RemoveAllDynamics();
	Super::EndPlay(EndPlay);
}