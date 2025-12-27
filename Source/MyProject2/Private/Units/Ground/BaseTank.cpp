// Fill out your copyright notice in the Description page of Project Settings.

#include "Units/Ground/BaseTank.h"

ABaseTank::ABaseTank() 
{

}

void ABaseTank::BeginPlay()
{
	Super::BeginPlay();
}

void ABaseTank::Tick(float DeltaSeconds) 
{
	Super::Tick(DeltaSeconds);
}

void ABaseTank::PossessedBy(AController* InController) 
{
	Super::PossessedBy(InController);
}