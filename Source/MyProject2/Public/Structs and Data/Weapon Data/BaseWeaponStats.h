// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Enums/TargetTypes.h"
#include "BaseWeaponStats.generated.h"

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	Gun UMETA(DisplayName = "Gun"),
	IRMissile UMETA(DisplayName = "IRMissile"),
	RadarMissile UMETA(DisplayName = "RadarMissile"),
	Bomb UMETA(DisplayName = "Bomb")
};

UCLASS()
class MYPROJECT2_API UBaseWeaponStats : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Damage = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LifeTime = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Cooldown = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EWeaponType WeaponType = EWeaponType::Gun;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<ETargetType> SupportedTargetTypes{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName InGameName = NAME_None;
};
