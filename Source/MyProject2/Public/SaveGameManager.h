// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerSaveData.h"
#include "SaveGameManager.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYPROJECT2_API USaveGameManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	USaveGameManager();


	void CreateNewSave();

	void LoadGame();

	void SaveGame();

	UPlayerSaveData* GetSaveData() const { return SaveGameData; }

	void AddMoney(int Amount);

	TArray<FName> GetAircraftOwned();

	int ReturnMoney();

private:
	UPROPERTY()
	UPlayerSaveData* SaveGameData;

	FString SlotName = "PlayerSave";
	int32 UserIndex = 0;
};
