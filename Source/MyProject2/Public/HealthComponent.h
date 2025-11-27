// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

class ABaseUnit;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYPROJECT2_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UHealthComponent();

	void Setup(float Health);

	void ApplyDamage(float Amount, AActor* Weaapon, AActor* Launcher, AActor* Target);

	float GetHealth() const { return CurrentHealth; };

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnDeath, AActor*, Weapon, AActor*, Launcher, AActor*, Target);
	FOnDeath OnDeath;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDamage);
	FOnDamage OnDamage;

	ABaseUnit* Controller;

private:
	float CurrentHealth = 100.f;

	// TODO: If there's any differences in all guns, like a flat out 2x damage mode, change it here
	float GunMultiplier = 1.f;

	float MissileMultiplier = 1.f;

	bool isAlive = true;
};
