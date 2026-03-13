// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Structs and Data/Aircraft Data/AircraftData.h"
#include "Structs and Data/Aircraft Data/AircraftDatabase.h"
#include "AircraftSelectionComponent.generated.h"

class AAircraftPlayerController;
class AAircraftSelectionGamemode;
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

	void Setup(AAircraftPlayerController* InPlayer, 
		AAircraftSelectionGamemode* InGM, 
		UMenuManagerComponent* InMenu,
		TSubclassOf<UUserWidget> InClass);

	UFUNCTION()
	void HandleAircraftPicked(UAircraftData* Aircraft);

	void UpdateAircraft(const FName& Name);

	UFUNCTION()
	void SetAircraft(UAircraftData* Aircraft);

	UAircraftSelectionWidget* GetWidget() const { return AircraftSelectUI; };

	void SetWidget(const TSubclassOf<UUserWidget> InWidget) { SelectionWidget = InWidget; };

	void CloseAll();

private:

	UPROPERTY()
	TSubclassOf<UUserWidget> SelectionWidget = nullptr;

	UPROPERTY()
	TObjectPtr<AAircraftPlayerController> PC = nullptr;

	UPROPERTY()
	TObjectPtr<AAircraftSelectionGamemode> GM = nullptr;

	UPROPERTY()
	TObjectPtr<UAircraftDatabase> AircraftDatabase = nullptr;

	UPROPERTY()
	TObjectPtr<UPlayerGameInstance> GameInstance = nullptr;

	UPROPERTY()
	TObjectPtr<UAircraftSelectionWidget> AircraftSelectUI = nullptr;

	UPROPERTY()
	TObjectPtr<UMenuManagerComponent> MenuManager = nullptr;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override {
		CloseAll();
		Super::EndPlay(EndPlayReason);
	}
};
