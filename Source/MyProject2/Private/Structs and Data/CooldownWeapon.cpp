// Fill out your copyright notice in the Description page of Project Settings.


#include "Structs and Data/CooldownWeapon.h"

FCooldownWeapon::FCooldownWeapon() 
	: WeaponInstance(nullptr)
	, WeaponClass(nullptr)
	, bCanFire(true)
	, time(0.f)
	, cooldownTime(5.0f)
	, SocketName(NAME_None)

{

}

void FCooldownWeapon::UpdateCooldown(float DeltaTime) 
{
	time -= DeltaTime;
	if (time <= 0) 
	{
		bCanFire = true;
		time = 0;
	}
}