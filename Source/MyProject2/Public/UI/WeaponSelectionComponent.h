// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Weapons/BaseWeapon.h"
#include "Structs and Data/AircraftData.h"
#include "WeaponSelectionComponent.generated.h"

class AAircraftPlayerController;
class AAircraftSelectionGamemode;
class ACurrentPlayerState;
class UPlayerGameInstance;
class UMenuManagerComponent;
class UWeaponSelectionWidget;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYPROJECT2_API UWeaponSelectionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UWeaponSelectionComponent();
		
	void Setup(AAircraftPlayerController* InPlayer, AAircraftSelectionGamemode* InGM, ACurrentPlayerState* InPS, UMenuManagerComponent* InMenu);
	
	void AddAircraft(UAircraftData* Data);
	
	UPROPERTY()
	AAircraftPlayerController* PC;

	UPROPERTY()
	AAircraftSelectionGamemode* GM; 

	UPROPERTY()
	ACurrentPlayerState* PS; 

	UPROPERTY()
	UMenuManagerComponent* MenuManager;

	UAircraftData* Aircraft;

	UPROPERTY()
	UWeaponSelectionWidget* WeaponSelectUI;

	UPROPERTY()
	TSubclassOf<UUserWidget> SelectionWidget;

	int32 CurrentPylonIndex;

	TMap<FName, TSubclassOf<ABaseWeapon>> WeaponSelection;

	void WeaponSelectionMenu();

	UFUNCTION()
	void HandleWeaponPicked(TSubclassOf<ABaseWeapon> Weapon);

	UFUNCTION()
	void AddWeapon(TSubclassOf<ABaseWeapon> Weapon);

	void CheckWeaponLoop();

	void CloseAll();

protected:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override {
		CloseAll();
		Super::EndPlay(EndPlayReason);
	}
};
