// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Weapons/BaseWeapon.h"
#include "Specials/BaseSpecial.h"
#include "Structs and Data/Aircraft Data/AircraftStats.h"
#include "PlayerGameInstance.generated.h"

class UMainMenuManager;
class ABaseAircraft;

UCLASS()
class MYPROJECT2_API UPlayerGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:

	UPROPERTY()
	class USaveGameManager* SaveManager;

	UPROPERTY()
	TSubclassOf<ABaseAircraft> AircraftClass;

	UPROPERTY()
	UAircraftStats* SelectedAircraftStats;

	UPROPERTY()
	TMap<FName, TSubclassOf<ABaseWeapon>> SelectedWeapons;

	UPROPERTY()
	TSubclassOf<UBaseSpecial> SelectedSpecial;

	UPROPERTY()
	UMainMenuManager* MainMenuManager;

	UMainMenuManager* GetMainMenuManager() const { return MainMenuManager; };

	FName LevelName;

	void SetLevel(const FName InLevel) { LevelName = InLevel; };

	void SetClass(TSubclassOf<ABaseAircraft> InClass) { AircraftClass = InClass; };

	void SetWeapons(TMap<FName, TSubclassOf<ABaseWeapon>> InWeapons) { SelectedWeapons = InWeapons; };

	void SetSpecial(TSubclassOf<UBaseSpecial> InSpecial) { SelectedSpecial = InSpecial; };

protected:
	void Init() override;

};
