// Fill out your copyright notice in the Description page of Project Settings.


#include "Gamemodes/CurrentPlayerState.h"

ACurrentPlayerState::ACurrentPlayerState() 
{

}

void ACurrentPlayerState::SetCurrentAircraft(UAircraftData* Aircraft) 
{
	SelectedAircraft = Aircraft;
}

void ACurrentPlayerState::AddWeapon(FName Pylon, TSubclassOf<ABaseWeapon> Weapon) 
{
	SelectedWeapons.Add(Pylon, *Weapon);
}

void ACurrentPlayerState::SetSpecial(TSubclassOf<UBaseSpecial> Special)
{
	SelectedSpecial = Special;
}