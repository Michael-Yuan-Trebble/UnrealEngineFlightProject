// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Aircraft/Player/PlayerAircraft.h"
#include "Aircraft/AI/F16AI.h"
#include "Structs and Data/Aircraft Data/AircraftDatabase.h"
#include "Structs and Data/Aircraft Data/AircraftData.h"
#include "FlightGamemode.generated.h"

class AT38Pawn;
class AAircraftPlayerController;
class ACurrentPlayerState;
class AAircraftRegistry;
class ABaseWeapon;

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
	
	UAircraftData* AircraftSelected;

	UAircraftDatabase* Database;

	UPROPERTY()
	APlayerAircraft* PlayerSpawnedIn;

	UPROPERTY()
	AAircraftPlayerController* PC;

	FTimerHandle TempHandle;

	UPROPERTY()
	ACurrentPlayerState* PS;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TSubclassOf<APlayerAircraft> Player;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AF16AI> AIAircraftClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<ABaseWeapon> Missile;

	UPROPERTY(EditAnywhere)
	TSubclassOf<ABaseWeapon> Bomb;

	virtual void BeginPlay() override;

private:
	void FallBackAircraft();

	TMap<FName,TSubclassOf<ABaseWeapon>> TemporaryLoadout();
};
