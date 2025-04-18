// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CurrentPlayerState.h"
#include "BaseAircraft.h"
#include "F16AI.h"
#include "FlightGamemode.generated.h"

class ASu25Pawn;
class AAircraftPlayerController;

UCLASS()
class MYPROJECT2_API AFlightGamemode : public AGameModeBase
{
	GENERATED_BODY()

public: 
	
	AFlightGamemode();

	virtual void BeginPlay() override;

	void HandlePlayerState(AAircraftPlayerController* PlayerControl, ABaseAircraft* ControlledAir);
	
	void SpawnAIAircraft();

protected:

	FTimerHandle TimerHandle;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TSubclassOf<ASu25Pawn> Player;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AF16AI> AIAircraftClass;
};
