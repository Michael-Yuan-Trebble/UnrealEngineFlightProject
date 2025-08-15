// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Structs and Data/AircraftDatabase.h"
#include "Structs and Data/AircraftData.h"
#include "AircraftSelectionGamemode.generated.h"

class UAircraftSelectionWidget;
class ACurrentPlayerState;
class AAircraftPlayerController;
class UMenuManagerComponent;

UCLASS()
class MYPROJECT2_API AAircraftSelectionGamemode : public AGameModeBase
{
	GENERATED_BODY()
	
public:

	AAircraftSelectionGamemode();

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UAircraftData* ChosenAircraft;

	UPROPERTY()
	AAircraftPlayerController* APC;

	UPROPERTY()
	ACurrentPlayerState* PS;
	
	UPROPERTY()
	UMenuManagerComponent* MenuManager;

	void SpawnInAircraft(TSubclassOf<APawn> SpawnIn);

	APawn* AircraftDisplayed;

	void SpawnInWeapon(TSubclassOf<ABaseWeapon> Weapon, FName Pylon);

	UPROPERTY()
	FName MapSelected;

	TMap<FName, AActor*> EquippedWeapons;

	void EndSelection(AAircraftPlayerController* Controller);

	void TryAdvanceToNextStage();

	int PlayersRequired = 1;

	TSet<AAircraftPlayerController*> ReadyPlayers;

protected:
	virtual void BeginPlay() override;
};
