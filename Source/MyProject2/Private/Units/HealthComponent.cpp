// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Health Component!"));
#include "Units/HealthComponent.h"
#include "Units/BaseUnit.h"
#include "Units/Aircraft/Player/PlayerAircraft.h"

UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UHealthComponent::Setup(float Health) 
{
	CurrentHealth = Health;
}

void UHealthComponent::ApplyDamage(float Damage, AActor* Weapon, AActor* Launcher, AActor* Target)
{
	if (!isAlive) return;
	CurrentHealth -= Damage * GunMultiplier * MissileMultiplier;

	if (CurrentHealth <= 0.f) 
	{
		isAlive = false;
		OnDeath.Broadcast(Weapon, Launcher, Target);
	}
	else OnDamage.Broadcast();

	UAircraftRegistry* Reg = UAircraftRegistry::Get(GetWorld());
	if (!Reg) return;
	Reg->Unregister(Controller);

	if (isAlive) Reg->OnAnyUnitHit.Broadcast(Launcher);
	else Reg->OnAnyUnitDeath.Broadcast(Launcher);
}