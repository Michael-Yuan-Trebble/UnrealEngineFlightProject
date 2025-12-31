// Fill out your copyright notice in the Description page of Project Settings.


#include "Gamemodes/TakeoffGamemode.h"

ATakeoffGamemode::ATakeoffGamemode() {
	FlightMode = EFlightMode::Gears;
}

void ATakeoffGamemode::BeginPlay() {
	Super::BeginPlay();
}