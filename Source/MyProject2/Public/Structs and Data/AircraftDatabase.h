// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AircraftData.h"
#include "AircraftDatabase.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT2_API UAircraftDatabase : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TArray<UAircraftData*> AllAircraft;

	void LoadAllAircraftFromFolder(FString Path);
};
