// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Flare!"));
#include "Specials/Countermeasures.h"
#include "Aircraft/BaseAircraft.h"
#include "Kismet/GameplayStatics.h"

void UCountermeasures::ActivateSpecial(ABaseAircraft* In) {
	if (!In) return;
	In->OnCountermeasureDeployed_Implementation();
	CurrentCooldown = CooldownTime;
}

bool UCountermeasures::CanActivate() { return CurrentCooldown == 0; }