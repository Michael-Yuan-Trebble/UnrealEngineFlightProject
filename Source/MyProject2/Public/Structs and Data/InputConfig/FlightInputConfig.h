// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "EnhancedInputComponent.h"
#include "FlightInputConfig.generated.h"

UCLASS(BlueprintType)
class MYPROJECT2_API UFlightInputConfig : public UDataAsset
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputAction> IA_Throttle = nullptr;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputAction> IA_Pitch = nullptr;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputAction> IA_Roll = nullptr;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputAction> IA_Yaw = nullptr;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputAction> IA_Shoot = nullptr;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputAction> IA_Weapons = nullptr;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputAction> IA_LookX = nullptr;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputAction> IA_LookY = nullptr;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputAction> IA_Focus = nullptr;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputAction> IA_Zoom = nullptr;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputAction> IA_NextWeapon = nullptr;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputAction> IA_PrevWeapon = nullptr;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputAction> IA_Menu = nullptr;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputAction> IA_Switch = nullptr;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputAction> IA_TogglePerspective = nullptr;
};
