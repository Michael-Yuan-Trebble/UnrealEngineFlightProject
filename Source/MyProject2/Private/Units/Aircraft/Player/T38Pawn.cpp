// Fill out your copyright notice in the Description page of Project Settings.


#include "Units/Aircraft/Player/T38Pawn.h"

AT38Pawn::AT38Pawn() 
{
	PrimaryActorTick.bCanEverTick = true;
}

void AT38Pawn::PossessedBy(AController* NewController) 
{
	Super::PossessedBy(NewController);
}
