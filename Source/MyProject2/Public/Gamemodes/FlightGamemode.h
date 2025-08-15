// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Aircraft/BaseAircraft.h"
#include "Aircraft/AI/F16AI.h"
#include "Structs and Data/AircraftDatabase.h"
#include "Structs and Data/AircraftData.h"
#include "FlightGamemode.generated.h"

class AT38Pawn;
class AAircraftPlayerController;
class ACurrentPlayerState;

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
	ABaseAircraft* PlayerSpawnedIn;

	UPROPERTY()
	AAircraftPlayerController* PC;

	FTimerHandle TempHandle;

	UPROPERTY()
	ACurrentPlayerState* PS;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TSubclassOf<AT38Pawn> Player;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AF16AI> AIAircraftClass;

	virtual void BeginPlay() override;
};
