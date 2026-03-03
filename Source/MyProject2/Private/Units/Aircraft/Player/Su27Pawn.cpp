// Fill out your copyright notice in the Description page of Project Settings.

#include "Units/Aircraft/Player/Su27Pawn.h"


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