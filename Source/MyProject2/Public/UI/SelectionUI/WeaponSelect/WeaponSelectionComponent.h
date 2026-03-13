// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Structs and Data/Aircraft Data/AircraftData.h"
#include "WeaponSelectionComponent.generated.h"

class AAircraftPlayerController;
class AAircraftSelectionGamemode;
class UMenuManagerComponent;
class UWeaponSelectionWidget;
class ABaseWeapon;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYPROJECT2_API UWeaponSelectionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UWeaponSelectionComponent();
		
	void Setup(AAircraftPlayerController* InPlayer, AAircraftSelectionGamemode* InGM, UMenuManagerComponent* InMenu);
	
	void AddAircraft(UAircraftData* Data);

	void WeaponSelectionMenu();

	UFUNCTION()
	void HandleWeaponPicked(TSubclassOf<ABaseWeapon> Weapon);

	UFUNCTION()
	void AddWeapon(TSubclassOf<ABaseWeapon> Weapon);

	void CheckWeaponLoop();

	void CloseAll();

	UWeaponSelectionWidget* GetWeaponUI() const { return WeaponSelectUI; };

	void SetWeaponWidgetClass(TSubclassOf<UUserWidget> InUI) { SelectionWidget = InUI; };

private:

	int32 CurrentPylonIndex = 0;

	UPROPERTY()
	TMap<FName, TSubclassOf<ABaseWeapon>> WeaponSelection;

	UPROPERTY()
	TObjectPtr<AAircraftPlayerController> PC = nullptr;

	UPROPERTY()
	TObjectPtr<AAircraftSelectionGamemode> GM = nullptr;

	UPROPERTY()
	TObjectPtr<UMenuManagerComponent> MenuManager = nullptr;

	UPROPERTY()
	TObjectPtr<UAircraftData> Aircraft = nullptr;

	UPROPERTY()
	TObjectPtr<UWeaponSelectionWidget> WeaponSelectUI = nullptr;

	UPROPERTY()
	TSubclassOf<UUserWidget> SelectionWidget = nullptr;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override {
		CloseAll();
		Super::EndPlay(EndPlayReason);
	}
};
