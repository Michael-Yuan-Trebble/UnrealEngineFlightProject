// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/BaseWeapon.h"
#include "PylonLoadout.generated.h"

USTRUCT(BlueprintType)
struct MYPROJECT2_API FPylonLoadout
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FName PylonName = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<ABaseWeapon>> AllowedMissiles{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<ABaseWeapon>> AllowedBombs{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<ABaseWeapon>> AllowedMisc{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxWeapons = 0;
};
