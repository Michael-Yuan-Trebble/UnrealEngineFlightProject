// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PylonLoadout.h"
#include "AircraftData.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT2_API UAircraftData : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TSubclassOf<APawn> AircraftClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName AircraftName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Class;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TArray<FPylonLoadout> PylonLoadouts;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int NumOfPylons;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int price;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Description;
};
