// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Structs and Data/InGame/InGameAirStats.h"
#include "Structs and Data/LoadoutInfo/AircraftWeaponInfo.h"
#include "Structs and Data/Weapon Data/BulletStats.h"
#include "Structs and Data/Aircraft Data/AircraftAudioData.h"
#include "AircraftStats.generated.h"

UCLASS()
class MYPROJECT2_API UAircraftStats : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName AircraftName = NAME_None;

	// TODO: Flesh this out, plan is to have two types of planes, one-circle and two-circle performers aka rate vs instant turn
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxAOA = 0.f;

	UPROPERTY(EditAnywhere)
	FInGameAirStats InGameAirStats{};

	UPROPERTY(EditAnywhere)
	FAircraftWeaponInfo WeaponInfo{};

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UBulletStats> BulletStats = nullptr;

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UAircraftAudioData> AudioData = nullptr;
};
