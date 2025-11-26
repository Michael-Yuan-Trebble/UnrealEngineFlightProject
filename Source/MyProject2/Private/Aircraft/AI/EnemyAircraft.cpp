// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Enemy Stats!"));
#include "Aircraft/AI/EnemyAircraft.h"
#include "Aircraft/AI/EnemyAircraftAI.h"
#include "Aircraft/FlightComponent.h"

AEnemyAircraft::AEnemyAircraft() 
{
	Faction = EFaction::Enemy;
	health = 100;
}


void AEnemyAircraft::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	Controller = Cast<AEnemyAircraftAI>(NewController);
}

void AEnemyAircraft::HandleDestroyed() 
{
	print(text)
	Destroy();
}