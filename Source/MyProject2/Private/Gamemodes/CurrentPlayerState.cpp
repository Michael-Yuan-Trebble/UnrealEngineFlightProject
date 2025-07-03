// Fill out your copyright notice in the Description page of Project Settings.


#include "Gamemodes/CurrentPlayerState.h"
#include "Weapons/Missiles/R60.h"

ACurrentPlayerState::ACurrentPlayerState() {
	SelectedWeapons.Add(AR60::StaticClass());
}