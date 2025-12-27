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
class ACurrentPlayerState;
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

	void InitializePC(AAircraftPlayerController* InPC, ACurrentPlayerState* InPS);

	void GoBack(EMenuState Current);

	void GetWidgetClassForState(EMenuState State);

	UPROPERTY()
	AAircraftPlayerController* PC;

	UPROPERTY()
	ACurrentPlayerState* PS;

	UPROPERTY()
	AAircraftSelectionGamemode* GM;

	UPROPERTY()
	UPlayerGameInstance* GameInstance;

	UPROPERTY()
	UAircraftData* SelectedAircraft;

	UPROPERTY()
	UAircraftData* TempAircraft;

	UPROPERTY()
	TSubclassOf<UBaseSpecial> SelectedSpecial;

	UPROPERTY()
	TSubclassOf<UUserWidget> AircraftSelectClass;

	UPROPERTY()
	TSubclassOf<UUserWidget> WeaponSelectClass;

	UPROPERTY()
	TSubclassOf<UUserWidget> BuySelectionClass;

	UPROPERTY()
	TSubclassOf<UUserWidget> SpecialSelectionClass;

	UPROPERTY()
	UUserWidget* CurrentWidget;

	UPROPERTY()
	UAircraftSelectionComponent* AircraftSelectionUI;

	UPROPERTY()
	UWeaponSelectionComponent* WeaponSelectionUI;

	UPROPERTY()
	UBuySelectionComponent* BuySelectionUI;

	UPROPERTY()
	USpecialSelectionComponent* SpecialSelectionUI;

	void ChooseAircraftUI();

	void ChooseWeaponUI();

	void ChooseSpecialUI();

	void EndSelection();

	void SetupClasses(TSubclassOf<UUserWidget> InAircraftClass,
		TSubclassOf<UUserWidget> InWeaponClass,
		TSubclassOf<UUserWidget> InBuyClass,
		TSubclassOf<UUserWidget> InSpecialClass);

	UFUNCTION()
	void SpawnBuy(UAircraftData* AircraftData, int Cost);

	UFUNCTION()
	void AdvanceToLevel();

	void CloseAll();

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override {
		CloseAll();
		Super::EndPlay(EndPlayReason);
	}
};
