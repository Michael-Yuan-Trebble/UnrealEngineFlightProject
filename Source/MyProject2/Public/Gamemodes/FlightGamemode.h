// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Structs and Data/Aircraft Data/AircraftDatabase.h"
#include "Structs and Data/Aircraft Data/AircraftData.h"
#include "Enums/FlightMode.h"
#include "FlightGamemode.generated.h"

class APlayerAircraft;
class AAircraftPlayerController;
class AAircraftRegistry;
class ABaseWeapon;
class UBaseSpecial;

UCLASS()
class MYPROJECT2_API AFlightGamemode : public AGameModeBase
{
	GENERATED_BODY()

public: 
	
	AFlightGamemode();

	void HandlePlayerState();
	
	void SpawnAIAircraft();

protected:

	FTimerHandle TimerHandle;
	
	UPROPERTY()
	TObjectPtr<UAircraftData> AircraftSelected = nullptr;

	UPROPERTY()
	TObjectPtr<UAircraftDatabase> Database = nullptr;

	UPROPERTY()
	TObjectPtr<APlayerAircraft> PlayerSpawnedIn = nullptr;

	UPROPERTY()
	TObjectPtr<AAircraftPlayerController> PC = nullptr;

	FTimerHandle TempHandle;

	UPROPERTY(EditAnywhere)
	TSubclassOf<APlayerAircraft> Player = nullptr;

	UPROPERTY(EditAnywhere)
	TSubclassOf<ABaseWeapon> Missile = nullptr;

	UPROPERTY(EditAnywhere)
	TSubclassOf<ABaseWeapon> Bomb = nullptr;

	UPROPERTY(EditAnywhere) 
	TSubclassOf<UBaseSpecial> Special = nullptr;

	UPROPERTY(EditAnywhere)
	float PlayerSpawnSpeed = 0.f;

	UPROPERTY(EditAnywhere)
	EFlightMode FlightMode = EFlightMode::Flight;

	bool bFinished = false;

	FTransform PlayerSpawnLoc = FTransform::Identity;

protected:
	void FallBackAircraft();

	TMap<FName,TSubclassOf<ABaseWeapon>> TemporaryLoadout();

	void SpawnInController();

	virtual void SetFlightMode();

	virtual void FinishMission();

	virtual void HandlePlayerSpawnPoint();

	virtual void BeginPlay() override;

	void SetPlayerSpeed();
};
