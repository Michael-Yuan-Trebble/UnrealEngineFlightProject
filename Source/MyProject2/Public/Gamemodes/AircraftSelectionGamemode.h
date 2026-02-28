// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Structs and Data/Aircraft Data/AircraftDatabase.h"
#include "Structs and Data/Aircraft Data/AircraftData.h"
#include "Structs and Data/AircraftLoadoutData.h"
#include "Enums/ControlModeTypes.h"
#include "AircraftSelectionGamemode.generated.h"

class UAircraftSelectionWidget;
class AAircraftPlayerController;
class UMenuManagerComponent;
class UAircraftSelectionWidget;

UCLASS()
class MYPROJECT2_API AAircraftSelectionGamemode : public AGameModeBase
{
	GENERATED_BODY()
	
public:

	AAircraftSelectionGamemode();

	void SpawnInAircraft(const TSubclassOf<APawn> SpawnIn);

	void SpawnInWeapon(const TSubclassOf<ABaseWeapon> Weapon, const FName& Pylon);

	void ClearWeapons(const FName& Pylon);

	void EndSelection(AAircraftPlayerController* Controller);

	void TryAdvanceToNextStage();

private:
	UPROPERTY()
	TObjectPtr<APawn> AircraftDisplayed = nullptr;

	UPROPERTY()
	TMap<FName, AActor*> EquippedWeapons;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> AircraftSelectClass = nullptr;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> WeaponSelectClass = nullptr;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> BuySelectionClass = nullptr;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> SpecialSelectionClass = nullptr;

	UPROPERTY()
	TObjectPtr<UAircraftData> ChosenAircraft = nullptr;

	UPROPERTY()
	TObjectPtr<AAircraftPlayerController> APC = nullptr;

	UPROPERTY()
	TObjectPtr<UMenuManagerComponent> MenuManager = nullptr;

	UPROPERTY()
	TSet<AAircraftPlayerController*> ReadyPlayers{};

	FAircraftLoadoutData FullLoadout{};

	virtual void BeginPlay() override;

	int PlayersRequired = 1;
};
