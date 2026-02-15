// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/BaseWeapon.h"
#include "CooldownWeapon.generated.h"

USTRUCT(BlueprintType)
struct FCooldownWeapon
{
	GENERATED_BODY()

	FCooldownWeapon();

	UPROPERTY(EditAnywhere)
	TSubclassOf <ABaseWeapon> WeaponClass = nullptr;

	UPROPERTY(EditAnywhere)
	ABaseWeapon* WeaponInstance = nullptr;

	UPROPERTY(EditAnywhere)
	bool bCanFire = true;

	UPROPERTY(EditAnywhere)
	FName SocketName = NAME_None;

	float time = 0.f;

	UPROPERTY(EditAnywhere)
	float cooldownTime = 0.f;

	bool CanFire() const { return bCanFire; }

	float ReturnCooldownTime() const { return time; };

	void StartCooldown() { bCanFire = false; time = cooldownTime; }

	void UpdateCooldown(const float DeltaTime);
};
