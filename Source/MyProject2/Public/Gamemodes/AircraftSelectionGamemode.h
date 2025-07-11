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
class AMenuManagerComponent;

UCLASS()
class MYPROJECT2_API AAircraftSelectionGamemode : public AGameModeBase
{
	GENERATED_BODY()
	
public:

	AAircraftSelectionGamemode();

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UAircraftData* ChosenAircraft;

	AAircraftPlayerController* APC;

	ACurrentPlayerState* PS;

	AMenuManagerComponent* MenuManager;

	virtual void BeginPlay() override;

	void SpawnInAircraft(TSubclassOf<APawn> SpawnIn);

	AActor* WeaponDisplayed;

	APawn* AircraftDisplayed;

	void SpawnInWeapon(TSubclassOf<ABaseWeapon> Weapon, FName Pylon);

	TMap<FName, TSubclassOf<AActor>> EquippedWeapons;
};
