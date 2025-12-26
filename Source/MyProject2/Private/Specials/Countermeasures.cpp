// Fill out your copyright notice in the Description page of Project Settings.


#include "Specials/Countermeasures.h"
#include "Kismet/GameplayStatics.h"

void UCountermeasures::ActivateSpecial() {
	OnFlareDeployed.Broadcast();
}

bool UCountermeasures::CanActivate() { return CurrentCooldown == 0; }