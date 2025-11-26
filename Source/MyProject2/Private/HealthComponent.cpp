// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthComponent.h"
#include "BaseUnit.h"

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
	CurrentHealth -= Damage;
	if (isAlive) 
	{
		if (CurrentHealth <= 0.f) 
		{
			isAlive = false;
			OnDeath.Broadcast(Weapon, Launcher, Target);
		}
		else 
		{
			OnDamage.Broadcast();
		}
	}
}