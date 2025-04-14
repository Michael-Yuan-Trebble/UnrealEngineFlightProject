// Fill out your copyright notice in the Description page of Project Settings.


#include "CooldownWeapon.h"

FCooldownWeapon::FCooldownWeapon() {

}

void FCooldownWeapon::UpdateCooldown(float DeltaTime) {
	time -= DeltaTime;
	if (time <= 0) {
		bCanFire = true;
		time = 0;
	}
}