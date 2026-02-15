// Fill out your copyright notice in the Description page of Project Settings.


#include "Player Info/PlayerGameState.h"

void APlayerGameState::RegisterKill(AActor* Killer, AActor* Target, const TSubclassOf<AActor> WeaponUsed) 
{
	if (!Target || !Killer) return;
	FKillEvent NewEvent;
	NewEvent.Killer = Killer;
	NewEvent.Victim = Target;
	NewEvent.WeaponUsed = WeaponUsed;
	NewEvent.Time = GetWorld()->GetTimeSeconds();

	KillLog.Add(NewEvent);
}