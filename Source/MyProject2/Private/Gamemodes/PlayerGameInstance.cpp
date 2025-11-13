// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Instance!"));
#include "Gamemodes/PlayerGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Gamemodes/MainMenuManager.h"
#include "SaveGameManager.h"

void UPlayerGameInstance::Init() 
{
	Super::Init();
	SaveManager = NewObject<USaveGameManager>(this, USaveGameManager::StaticClass());
	SaveManager->LoadGame();

	MainMenuManager = GetSubsystem<UMainMenuManager>();
}

void UPlayerGameInstance::SetLoadout(UAircraftData* InData, TMap<FName, TSubclassOf<ABaseWeapon>> InWeapons, TSubclassOf<UBaseSpecial> InSpecial) {
	SelectedAircraft = InData;

	SelectedWeapons = InWeapons;

	SelectedSpecial = InSpecial;
}