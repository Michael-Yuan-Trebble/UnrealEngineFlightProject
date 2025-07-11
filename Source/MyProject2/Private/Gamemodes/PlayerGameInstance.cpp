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