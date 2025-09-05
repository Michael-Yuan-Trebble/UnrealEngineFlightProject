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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf <ABaseWeapon> WeaponClass;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	ABaseWeapon* WeaponInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCanFire;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName SocketName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float time;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float cooldownTime;

	bool CanFire() {
		return bCanFire;
	}

	void StartCooldown() {
		bCanFire = false;
		time = cooldownTime;
	}

	void UpdateCooldown(float DeltaTime);
};
