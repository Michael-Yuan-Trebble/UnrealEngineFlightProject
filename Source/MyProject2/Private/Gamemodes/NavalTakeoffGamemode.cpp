// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Carrier!"));
#include "Gamemodes/NavalTakeoffGamemode.h"
#include "Units/Aircraft/Player/PlayerAircraft.h"
#include "Player Info/AircraftPlayerController.h"
#include "GameFramework/PlayerStart.h"
#include "Player Info/PlayerGameInstance.h"
#include "Units/Carrier.h"
#include "Enums/ThrottleStage.h"

ANavalTakeoffGamemode::ANavalTakeoffGamemode() {
	FlightMode = EFlightMode::Naval;
	PrimaryActorTick.bCanEverTick = true;
}
void ANavalTakeoffGamemode::BeginPlay(){
	Super::BeginPlay();
}

void ANavalTakeoffGamemode::Tick(float D) {
	Super::Tick(D);
	CatapultTakeoff(D);
	CheckTakeoff(D);
}

// Spawn in Carrier and get the point
void ANavalTakeoffGamemode::HandlePlayerSpawnPoint() 
{
	if (CarrierClass && IsValid(GetWorld()))
	{
		FTransform SpawnTransform;
		SpawnTransform.SetTranslation(SpawnVector);
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		Carrier = GetWorld()->SpawnActor<ACarrier>(CarrierClass, SpawnTransform, Params);

		if (IsValid(Carrier)) PlayerSpawnLoc = Carrier->GetCarrierSpawnPoint();

		PlayerSpawnSpeed = 0.f;
	}
}

void ANavalTakeoffGamemode::CatapultTakeoff(const float D) {
	if (!IsValid(PlayerSpawnedIn)) return;

	if (bStatic) {
		EThrottleStage Throttle = PlayerSpawnedIn->GetThrottleStage();
		if (Throttle == EThrottleStage::Afterburner) {
			CurrentCatapultTime -= D;

		}
		else CurrentCatapultTime = CatapultTimer;
	}

	if (CurrentCatapultTime <= 0 && LaunchTime > 0) {
		LaunchTime -= D;
		PlayerSpawnedIn->ApplySpeed(AddedSpeed, D);
		if (LaunchTime <= 0) {
			PlayerSpawnedIn->SetFlying(true);
		}
	}
}

void ANavalTakeoffGamemode::CheckTakeoff(const float D) 
{
	if (!IsValid(PlayerSpawnedIn)) return;
	FVector PlayerLoc = PlayerSpawnedIn->GetActorLocation();
	float Dist = FVector::Dist(PlayerLoc, PlayerSpawnLoc.GetLocation());
	if (Dist >= Distance) 
	{
		TakeoffTimer -= D;
		if (TakeoffTimer <= 0) 
		{
			TransitionLevel();
		}
	}
}

void ANavalTakeoffGamemode::TransitionLevel() {
	if (!PC || bFinished) return;
	bFinished = true;
	PC->ClientMessage(TEXT("COMPLETE"));
	PC->DeactivateMovement();
	PC->DeactivateWeapon();
	UPlayerGameInstance* GI = GetWorld()->GetGameInstance<UPlayerGameInstance>();
	if (!IsValid(GI)) return;
	GI->FadeIn();
}