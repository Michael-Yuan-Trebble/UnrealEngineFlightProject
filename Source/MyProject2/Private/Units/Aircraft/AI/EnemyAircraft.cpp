// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Enemy Stats!"));
#include "Units/Aircraft/AI/EnemyAircraft.h"
#include "Units/Aircraft/AI/EnemyAircraftAI.h"
#include "Units/Aircraft/AI/AIFlightComponent.h"
#include "Units/Aircraft/FlightComponent.h"
#include "Units/Aircraft/WeaponSystemComponent.h"

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

void AEnemyAircraft::FireBullets() { if (WeaponComponent) WeaponComponent->FireBullets(); }

void AEnemyAircraft::StartBullets() 
{
	FireBullets();
	GetWorld()->GetTimerManager().SetTimer(RepeatTimerHandle, this, &AEnemyAircraft::FireBullets, BulletStats->FireRate, true);
}

void AEnemyAircraft::EndBullets() 
{
	GetWorld()->GetTimerManager().ClearTimer(RepeatTimerHandle);
}

void AEnemyAircraft::SwitchWeapon(const TSubclassOf<ABaseWeapon> InWeapon)
{
	if (WeaponComponent) WeaponComponent->SearchAndEquipWeapon(InWeapon);
}