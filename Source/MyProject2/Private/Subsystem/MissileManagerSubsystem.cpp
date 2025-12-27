// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/MissileManagerSubsystem.h"

void UMissileManagerSubsystem::RegisterMissile(ABaseMissile* InMissile) 
{
	ActiveMissiles.Add(InMissile);
}

void UMissileManagerSubsystem::UnregisterMissile(ABaseMissile* InMissile) 
{
	ActiveMissiles.Remove(InMissile);
}