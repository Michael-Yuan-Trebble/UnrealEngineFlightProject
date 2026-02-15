// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Enums/MenuState.h"
#include "Structs and Data/Aircraft Data/AircraftData.h"
#include "Specials/BaseSpecial.h"
#include "Blueprint/UserWidget.h"
#include "MenuManagerComponent.generated.h"

class AAircraftPlayerController;
class AAircraftSelectionGamemode;
class UPlayerGameInstance;
class UAircraftSelectionComponent;
class UWeaponSelectionComponent;
class UBuySelectionComponent;
class USpecialSelectionComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYPROJECT2_API UMenuManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UMenuManagerComponent();

	void InitializePC(AAircraftPlayerController* InPC);

	void GoBack(const EMenuState Current);

	void GetWidgetClassForState(const EMenuState State);

	UPROPERTY()
	AAircraftPlayerController* PC = nullptr;

	UPROPERTY()
	AAircraftSelectionGamemode* GM = nullptr;

	UPROPERTY()
	UPlayerGameInstance* GameInstance = nullptr;

	UPROPERTY()
	UAircraftData* SelectedAircraft = nullptr;

	UPROPERTY()
	UAircraftData* TempAircraft = nullptr;

	UPROPERTY()
	TSubclassOf<UBaseSpecial> SelectedSpecial = nullptr;

	UPROPERTY()
	TSubclassOf<UUserWidget> AircraftSelectClass = nullptr;

	UPROPERTY()
	TSubclassOf<UUserWidget> WeaponSelectClass = nullptr;

	UPROPERTY()
	TSubclassOf<UUserWidget> BuySelectionClass = nullptr;

	UPROPERTY()
	TSubclassOf<UUserWidget> SpecialSelectionClass = nullptr;

	UPROPERTY()
	UUserWidget* CurrentWidget = nullptr;

	UPROPERTY()
	UAircraftSelectionComponent* AircraftSelectionUI = nullptr;

	UPROPERTY()
	UWeaponSelectionComponent* WeaponSelectionUI = nullptr;

	UPROPERTY()
	UBuySelectionComponent* BuySelectionUI = nullptr;

	UPROPERTY()
	USpecialSelectionComponent* SpecialSelectionUI = nullptr;

	void ChooseAircraftUI();

	void ChooseWeaponUI();

	void ChooseSpecialUI();

	void EndSelection();

	void SetupClasses(TSubclassOf<UUserWidget> InAircraftClass,
		TSubclassOf<UUserWidget> InWeaponClass,
		TSubclassOf<UUserWidget> InBuyClass,
		TSubclassOf<UUserWidget> InSpecialClass);

	UFUNCTION()
	void SpawnBuy(UAircraftData* AircraftData, const int Cost);

	UFUNCTION()
	void AdvanceToLevel();

	void CloseAll();

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override {
		CloseAll();
		Super::EndPlay(EndPlayReason);
	}
};
