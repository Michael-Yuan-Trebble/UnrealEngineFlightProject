// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Structs and Data/Aircraft Data/AircraftStats.h"
#include "AIAircraftData.generated.h"

UCLASS(BlueprintType)
class MYPROJECT2_API UAIAircraftData : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TSubclassOf<APawn> AircraftClass = nullptr;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FName AircraftName = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Class = FText::GetEmpty();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAircraftStats> Stats = nullptr;
};
