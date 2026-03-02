// Fill out your copyright notice in the Description page of Project Settings.


#include "Structs and Data/CooldownWeapon.h"

FCooldownWeapon::FCooldownWeapon() {}

void FCooldownWeapon::UpdateCooldown(const float DeltaTime) 
{
	time = FMath::Max(0, time-DeltaTime);
	bCanFire = (time <= 0);
}