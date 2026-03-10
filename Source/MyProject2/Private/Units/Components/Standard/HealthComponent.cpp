// Fill out your copyright notice in the Description page of Project Settings.

#include "Units/Components/Standard/HealthComponent.h"
#include "Units/BaseUnit.h"
#include "Subsystem/AircraftRegistry.h"
#include "Units/Aircraft/Player/PlayerAircraft.h"

UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UHealthComponent::Setup(const float Health)
{
	CurrentHealth = Health;
}

void UHealthComponent::ApplyDamage(const float Damage, AActor* Weapon, AActor* Launcher, AActor* Target)
{
	if (!isAlive) return;
	CurrentHealth -= Damage * GunMultiplier * MissileMultiplier;

	if (CurrentHealth <= 0.f) 
	{
		isAlive = false;
		OnDeath.Broadcast(Weapon, Launcher, Target);
	}
	else OnDamage.Broadcast();

	if (UAircraftRegistry* Reg = UAircraftRegistry::Get(GetWorld())) {
		Reg->Unregister(Controller);

		if (isAlive) Reg->OnAnyUnitHit.Broadcast(Launcher);
		else Reg->OnAnyUnitDeath.Broadcast(Launcher);
	}
}