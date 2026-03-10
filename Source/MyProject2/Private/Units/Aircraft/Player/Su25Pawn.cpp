// Fill out your copyright notice in the Description page of Project Settings.

#include "Units/Aircraft/Player/Su25Pawn.h"

//Initialize Su25 Player Pawn
ASu25Pawn::ASu25Pawn() 
{
	PrimaryActorTick.bCanEverTick = true;
}
void ASu25Pawn::PossessedBy(AController* NewController) 
{
	Super::PossessedBy(NewController);
}