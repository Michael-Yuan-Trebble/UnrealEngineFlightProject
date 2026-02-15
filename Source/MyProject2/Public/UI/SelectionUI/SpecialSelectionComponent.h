// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Specials/BaseSpecial.h"
#include "Structs and Data/Aircraft Data/AircraftData.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "SpecialSelectionComponent.generated.h"

class AAircraftPlayerController;
class AAircraftSelectionGamemode;
class UPlayerGameInstance;
class UMenuManagerComponent;
class USpecialSelectionWidget;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYPROJECT2_API USpecialSelectionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	USpecialSelectionComponent();

	void SpecialSelectionMenu();

	void Setup(AAircraftPlayerController* InPlayer, UMenuManagerComponent* InMenu);

	UFUNCTION()
	void SetSpecial(TSubclassOf<UBaseSpecial> InSpecial);

	UFUNCTION()
	void AdvanceLevel();

	void SetAir(UAircraftData* InAir);

	UPROPERTY()
	TSubclassOf<UUserWidget> SelectionWidget = nullptr;

	UPROPERTY()
	AAircraftPlayerController* PC = nullptr;

	UPROPERTY()
	UPlayerGameInstance* GameInstance = nullptr;

	UPROPERTY()
	USpecialSelectionWidget* SpecialSelectUI = nullptr;

	UPROPERTY()
	UMenuManagerComponent* MenuManager = nullptr;

	UPROPERTY()
	UAircraftData* SelectedAircraft = nullptr;

	void CloseAll();

protected:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override 
	{
		CloseAll();
		Super::EndPlay(EndPlayReason);
	}
};
