// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Missile!"));
#include "BaseIRMissile.h"

//Initialize Base Missile
ABaseIRMissile::ABaseIRMissile()
{
	PrimaryActorTick.bCanEverTick = true;

	timeDet = 5;
	timeTilDet = 0;
	isAir = false;
}

void ABaseIRMissile::BeginPlay()
{
	Super::BeginPlay();
	
}

//If Missile is shot, delete after timeDet time
void ABaseIRMissile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (isAir) {
		timeTilDet += DeltaTime;
		if (timeTilDet >= timeDet) {
			Destroy();
		}
	}
}

void ABaseIRMissile::FireStatic(float launchSpeed)
{

}

void ABaseIRMissile::FireTracking(float launchSpeed, AActor* Target) {

}

float ABaseIRMissile::ReturnCooldownTime() {
	return cooldownTime;
}