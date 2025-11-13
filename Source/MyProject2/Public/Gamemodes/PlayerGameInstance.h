// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Weapons/BaseWeapon.h"
#include "Specials/BaseSpecial.h"
#include "Structs and Data/Aircraft Data/AircraftData.h"
#include "PlayerGameInstance.generated.h"

class UMainMenuManager;

UCLASS()
class MYPROJECT2_API UPlayerGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:

	UPROPERTY()
	class USaveGameManager* SaveManager;

	UAircraftData* SelectedAircraft;

	TMap<FName, TSubclassOf<ABaseWeapon>> SelectedWeapons;

	TSubclassOf<UBaseSpecial> SelectedSpecial;

	UMainMenuManager* MainMenuManager;

	UMainMenuManager* GetMainMenuManager() const { return MainMenuManager; };

protected:
	void Init() override;

	void SetLoadout(UAircraftData* InData, TMap<FName, TSubclassOf<ABaseWeapon>> InWeapons,TSubclassOf<UBaseSpecial> InSpecial);
};
