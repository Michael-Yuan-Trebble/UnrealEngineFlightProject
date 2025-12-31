// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Su27"));
#include "Units/Aircraft/Player/Su27Pawn.h"
#include "Units/Aircraft/FlightComponent.h"
#include "Player Info/AircraftPlayerController.h"

ASu27Pawn::ASu27Pawn()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ASu27Pawn::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
}

void ASu27Pawn::Tick(float DeltaSeconds) 
{
	Super::Tick(DeltaSeconds);
}