// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Structs and Data/Weapon Data/BaseWeaponStats.h"
#include "BulletStats.generated.h"

class AAircraftBullet;

UCLASS()
class MYPROJECT2_API UBulletStats : public UBaseWeaponStats
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere)
	TSubclassOf<AAircraftBullet> BulletClass;

	UPROPERTY(EditAnywhere)
	float FireRate;
};
