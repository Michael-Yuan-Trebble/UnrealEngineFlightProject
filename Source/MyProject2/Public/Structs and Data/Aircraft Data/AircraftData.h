// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Structs and Data/PylonLoadout.h"
#include "Specials/BaseSpecial.h"
#include "Structs and Data/Aircraft Data/AircraftStats.h"
#include "AircraftData.generated.h"

UCLASS()
class MYPROJECT2_API UAircraftData : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TSubclassOf<APawn> AircraftClass = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Class;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TArray<FPylonLoadout> PylonLoadouts;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int NumOfPylons = 0;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int price = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<UBaseSpecial>> Specials;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAircraftStats* AircraftStat = nullptr;
};
