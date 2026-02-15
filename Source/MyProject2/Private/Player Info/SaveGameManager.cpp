// Fill out your copyright notice in the Description page of Project Settings.


#include "Player Info/SaveGameManager.h"
#include "Kismet/GameplayStatics.h"

USaveGameManager::USaveGameManager()
{
	LoadGame();
}

void USaveGameManager::CreateNewSave() 
{
	SaveGameData = Cast<UPlayerSaveData>(UGameplayStatics::CreateSaveGameObject(UPlayerSaveData::StaticClass()));
	if (SaveGameData)
	{
		SaveGameData->Money = 0;
		UE_LOG(LogTemp, Warning, TEXT("Created new save data"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create new save data"));
	}
}

void USaveGameManager::SaveGame() 
{
	if (!SaveGameData) CreateNewSave();
	const bool bSuccess = UGameplayStatics::SaveGameToSlot(SaveGameData, SlotName, UserIndex);
	if (bSuccess)
	{
		UE_LOG(LogTemp, Warning, TEXT("Game saved successfully"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to save game"));
	}
}

void USaveGameManager::LoadGame() 
{
	if (UGameplayStatics::DoesSaveGameExist(SlotName, UserIndex)) 
	{
		SaveGameData = Cast<UPlayerSaveData>(UGameplayStatics::LoadGameFromSlot(SlotName, UserIndex));
		if (SaveGameData)
		{
			UE_LOG(LogTemp, Warning, TEXT("Save loaded successfully"));
		}
		else {
			UE_LOG(LogTemp, Error, TEXT("Failed to load"));
			CreateNewSave();
		}
	}
	else 
	{
		UE_LOG(LogTemp, Warning, TEXT("No save found, creating new one"));
		CreateNewSave();
	}
}

void USaveGameManager::AddMoney(const int Amount) 
{
	SaveGameData->Money += Amount;
	if (SaveGameData->Money < 0) 
	{
		SaveGameData->Money = 0;
	}
}

void USaveGameManager::AddAircraftOwned(const FName& AircraftAdd) 
{
	SaveGameData->AircraftOwned.Add(AircraftAdd);
}

TArray<FName> USaveGameManager::GetAircraftOwned() 
{
	TArray<FName> A;
	if (IsValid(SaveGameData)) return SaveGameData->AircraftOwned;
	return A;
}

int USaveGameManager::ReturnMoney()
{
	if (IsValid(SaveGameData)) return SaveGameData->Money;
	return 0;
}