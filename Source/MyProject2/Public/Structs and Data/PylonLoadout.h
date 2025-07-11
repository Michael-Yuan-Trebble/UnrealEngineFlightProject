// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/BaseWeapon.h"
#include "PylonLoadout.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct MYPROJECT2_API FPylonLoadout
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FName PylonName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<TSubclassOf<ABaseWeapon>> AllowedMissiles;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<TSubclassOf<ABaseWeapon>> AllowedBombs;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<TSubclassOf<ABaseWeapon>> AllowedMisc;

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	int32 MaxWeapons;

	FPylonLoadout()
		: PylonName("") {};
};
