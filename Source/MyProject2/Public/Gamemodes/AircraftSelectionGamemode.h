// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Structs and Data/AircraftDatabase.h"
#include "Structs and Data/AircraftData.h"
#include "AircraftSelectionGamemode.generated.h"

class UAircraftSelectionWidget;

UCLASS()
class MYPROJECT2_API AAircraftSelectionGamemode : public AGameModeBase
{
	GENERATED_BODY()
	
public:

	AAircraftSelectionGamemode();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAircraftSelectionWidget* AircraftSelectUI;

	UPROPERTY()
	UAircraftDatabase* AircraftDatabase;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UUserWidget> WidgetClass;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UAircraftData* ChosenAircraft;

	UFUNCTION()
	void PickedAircraft(UAircraftData* Aircraft);

	virtual void BeginPlay() override;
};
