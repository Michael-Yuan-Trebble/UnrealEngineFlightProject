// Fill out your copyright notice in the Description page of Project Settings.

#include "Units/Aircraft/AI/EnemyAircraft.h"
#include "Units/Components/Aircraft/AIFlightComponent.h"
#include "AI/AircraftAIController.h"
#include "Units/Components/Aircraft/FlightComponent.h"
#include "Units/Components/Aircraft/WeaponSystemComponent.h"
#include "Debug/DebugHelper.h"

AEnemyAircraft::AEnemyAircraft() 
{
	Faction = EFaction::Enemy;
	health = 100;
}


void AEnemyAircraft::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	Controller = Cast<AAircraftAIController>(NewController);
}

void AEnemyAircraft::FireBullets() { if (IsValid(WeaponComponent)) WeaponComponent->FireBullets(); }

void AEnemyAircraft::StartBullets() 
{
	FireBullets();
	if (!IsValid(CachedBulletStats)) return;
	GetWorld()->GetTimerManager().SetTimer(RepeatTimerHandle, this, &AEnemyAircraft::FireBullets, CachedBulletStats->FireRate, true);
}

void AEnemyAircraft::EndBullets() 
{
	GetWorld()->GetTimerManager().ClearTimer(RepeatTimerHandle);
}