// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "EnhancedInputComponent.h"
#include "WeaponInputConfig.generated.h"

UCLASS(BlueprintType)
class MYPROJECT2_API UWeaponInputConfig : public UDataAsset
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputAction> IA_Shoot = nullptr;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputAction> IA_Weapons = nullptr;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputAction> IA_NextWeapon = nullptr;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputAction> IA_PrevWeapon = nullptr;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputAction> IA_Special = nullptr;
};
