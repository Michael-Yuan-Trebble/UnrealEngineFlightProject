// Fill out your copyright notice in the Description page of Project Settings.


#include "FlightGamemode.h"
#include "UObject/ConstructorHelpers.h"
#include "GameFramework/Actor.h"

AFlightGamemode::AFlightGamemode() {

	//Find blueprint location, then set it to a variable
	//static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/Blueprints/C++Player"));
	//DefaultPawnClass = PlayerPawnClassFinder.Class;
}

void AFlightGamemode::BeginPlay() {
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("Gamemode"));
}