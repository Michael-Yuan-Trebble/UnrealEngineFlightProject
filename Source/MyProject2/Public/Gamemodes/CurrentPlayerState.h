// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Weapons/BaseWeapon.h"
#include "Specials/BaseSpecial.h"
#include "Structs and Data/AircraftData.h"
#include "CurrentPlayerState.generated.h"

UCLASS()
class MYPROJECT2_API ACurrentPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	ACurrentPlayerState();

	UAircraftData* SelectedAircraft;

	TMap<FName, TSubclassOf<ABaseWeapon>> SelectedWeapons;

	TSubclassOf<UBaseSpecial> SelectedSpecial;

	void SetCurrentAircraft(UAircraftData* Aircraft);

	void AddWeapon(FName Pylon, TSubclassOf<ABaseWeapon> Weapon);

	void SetSpecial(TSubclassOf<UBaseSpecial> Special);
};
