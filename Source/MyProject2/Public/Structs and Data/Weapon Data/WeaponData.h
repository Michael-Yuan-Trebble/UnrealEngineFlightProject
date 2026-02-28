// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Structs and Data/Weapon Data/BaseWeaponStats.h"
#include "WeaponData.generated.h"

UCLASS()
class MYPROJECT2_API UWeaponData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> WeaponMesh = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText DisplayName = FText::GetEmpty();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Description = FText::GetEmpty();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UBaseWeaponStats> WeaponStat = nullptr;

};
