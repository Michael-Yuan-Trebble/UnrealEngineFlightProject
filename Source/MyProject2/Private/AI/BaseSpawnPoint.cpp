// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BaseSpawnPoint.h"
#include "Gamemodes/StandardMissionGamemode.h"

void ABaseSpawnPoint::ActivateSpawn() {
	AStandardMissionGamemode* GM = Cast<AStandardMissionGamemode>(GetWorld()->GetAuthGameMode());
	if (IsValid(GM)) {
		
	}
}