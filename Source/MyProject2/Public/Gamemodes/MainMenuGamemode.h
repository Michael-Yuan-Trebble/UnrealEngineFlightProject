// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Player Info/CurrentPlayerState.h"
#include "MainMenuGamemode.generated.h"

class AAircraftPlayerController;
class UMainMenuWidget;
class UFreeFlightWidget;

UCLASS()
class MYPROJECT2_API AMainMenuGamemode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	AMainMenuGamemode();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	AAircraftPlayerController* APC;

	UPROPERTY()
	ACurrentPlayerState* PS;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UMainMenuWidget> MainMenuClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UFreeFlightWidget> FreeFlightClass;
};
