// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Structs and Data/Aircraft Data/AircraftDatabase.h"
#include "Structs and Data/Aircraft Data/AircraftData.h"
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

	void HandlePlayerState(AAircraftPlayerController* PlayerControl);
	
	void SpawnAIAircraft();

protected:

	FTimerHandle TimerHandle;
	
	UPROPERTY()
	UAircraftData* AircraftSelected;

	UPROPERTY()
	UAircraftDatabase* Database;

	UPROPERTY()
	APlayerAircraft* PlayerSpawnedIn;

	UPROPERTY()
	AAircraftPlayerController* PC;

	FTimerHandle TempHandle;

	UPROPERTY(EditAnywhere)
	TSubclassOf<APlayerAircraft> Player;

	UPROPERTY(EditAnywhere)
	TSubclassOf<ABaseWeapon> Missile;

	UPROPERTY(EditAnywhere)
	TSubclassOf<ABaseWeapon> Bomb;

	UPROPERTY(EditAnywhere) 
	TSubclassOf<UBaseSpecial> Special;

	UPROPERTY(EditAnywhere)
	float PlayerSpawnSpeed = 0.f;

	virtual void BeginPlay() override;

	void SetPlayerSpeed();

private:
	void FallBackAircraft();

	TMap<FName,TSubclassOf<ABaseWeapon>> TemporaryLoadout();

	void SpawnInController();

	void SetFlightMode();

	bool bTakingOff;
};
