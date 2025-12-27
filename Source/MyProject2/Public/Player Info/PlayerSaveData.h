// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "PlayerSaveData.generated.h"

UCLASS()
class MYPROJECT2_API UPlayerSaveData : public USaveGame
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, Category = "SaveData")
	TArray<FName> AircraftOwned;

	UPROPERTY(EditAnywhere, Category = "SaveData")
	int32 Money;
};
