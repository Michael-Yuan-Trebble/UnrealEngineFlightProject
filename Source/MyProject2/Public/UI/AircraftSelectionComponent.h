// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Structs and Data/AircraftData.h"
#include "Structs and Data/AircraftDatabase.h"
#include "AircraftSelectionComponent.generated.h"

class AAircraftPlayerController;
class AAircraftSelectionGamemode;
class ACurrentPlayerState;
class UPlayerGameInstance;
class UMenuManagerComponent;
class UAircraftSelectionWidget;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYPROJECT2_API UAircraftSelectionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UAircraftSelectionComponent();

	void AircraftSelectionMenu();

	void Setup(AAircraftPlayerController* InPlayer, AAircraftSelectionGamemode* InGM, ACurrentPlayerState* InPS, UMenuManagerComponent* InMenu);

	UFUNCTION()
	void HandleAircraftPicked(UAircraftData* Aircraft);

	void UpdateAircraft(FName Name);

	UFUNCTION()
	void SetAircraft(UAircraftData* Aircraft);

	UPROPERTY()
	TSubclassOf<UUserWidget> SelectionWidget;

	UPROPERTY()
	AAircraftPlayerController* PC;

	UPROPERTY()
	AAircraftSelectionGamemode* GM;

	UPROPERTY()
	ACurrentPlayerState* PS;

	UPROPERTY()
	UAircraftDatabase* AircraftDatabase;

	UPROPERTY()
	UPlayerGameInstance* GameInstance;

	UPROPERTY()
	UAircraftSelectionWidget* AircraftSelectUI;

	UPROPERTY()
	UMenuManagerComponent* MenuManager;

	void CloseAll();

protected:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override {
		Super::EndPlay(EndPlayReason);
		CloseAll();
	}
};
