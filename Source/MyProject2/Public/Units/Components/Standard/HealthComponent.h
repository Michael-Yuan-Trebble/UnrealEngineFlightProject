// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

class ABaseUnit;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnDeath, AActor*, Weapon, AActor*, Launcher, AActor*, Target);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDamage);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYPROJECT2_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UHealthComponent();

	void Setup(const float Health);

	void ApplyDamage(const float Amount, AActor* Weaapon, AActor* Launcher, AActor* Target);

	float GetHealth() const { return CurrentHealth; };

	FOnDeath OnDeath;

	FOnDamage OnDamage;

	void SetController(ABaseUnit* In) { Controller = In; };

private:

	UPROPERTY()
	TObjectPtr<ABaseUnit> Controller = nullptr;

	float CurrentHealth = 100.f;

	// TODO: If there's any differences in all guns, like a flat out 2x damage mode, change it here
	float GunMultiplier = 1.f;

	float MissileMultiplier = 1.f;

	bool isAlive = true;
};
