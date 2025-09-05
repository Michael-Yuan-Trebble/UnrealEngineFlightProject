// Fill out your copyright notice in the Description page of Project Settings.

#include "Gamemodes/PlayerGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "SaveGameManager.h"

void UPlayerGameInstance::Init() 
{
	Super::Init();
	SaveManager = NewObject<USaveGameManager>(this, USaveGameManager::StaticClass());
	SaveManager->LoadGame();
}

void UPlayerGameInstance::SetLoadout(UAircraftData* InData, TMap<FName, TSubclassOf<ABaseWeapon>> InWeapons, TSubclassOf<UBaseSpecial> InSpecial) {
	SelectedAircraft = InData;

	SelectedWeapons = InWeapons;

	SelectedSpecial = InSpecial;
}