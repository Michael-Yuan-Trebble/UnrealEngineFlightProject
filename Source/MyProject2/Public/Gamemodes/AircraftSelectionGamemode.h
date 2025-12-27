// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Structs and Data/Aircraft Data/AircraftDatabase.h"
#include "Structs and Data/Aircraft Data/AircraftData.h"
#include "AircraftSelectionGamemode.generated.h"

class UAircraftSelectionWidget;
class ACurrentPlayerState;
class AAircraftPlayerController;
class UMenuManagerComponent;
class UAircraftSelectionWidget;

UCLASS()
class MYPROJECT2_API AAircraftSelectionGamemode : public AGameModeBase
{
	GENERATED_BODY()
	
public:

	AAircraftSelectionGamemode();

	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> AircraftSelectClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> WeaponSelectClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> BuySelectionClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> SpecialSelectionClass;

	UPROPERTY()
	UAircraftData* ChosenAircraft;

	UPROPERTY()
	AAircraftPlayerController* APC;

	UPROPERTY()
	ACurrentPlayerState* PS;
	
	UPROPERTY()
	UMenuManagerComponent* MenuManager;

	void SpawnInAircraft(TSubclassOf<APawn> SpawnIn);

	UPROPERTY()
	APawn* AircraftDisplayed;

	void SpawnInWeapon(TSubclassOf<ABaseWeapon> Weapon, FName Pylon);

	void ClearWeapons(FName Pylon);

	UPROPERTY()
	TMap<FName, AActor*> EquippedWeapons;

	void EndSelection(AAircraftPlayerController* Controller);

	void TryAdvanceToNextStage();

	int PlayersRequired = 1;

	UPROPERTY()
	TSet<AAircraftPlayerController*> ReadyPlayers;

protected:
	virtual void BeginPlay() override;
};
