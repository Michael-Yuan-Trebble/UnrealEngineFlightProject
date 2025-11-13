// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Gamemodes/CurrentPlayerState.h"
#include "MainMenuGamemode.generated.h"

class AAircraftPlayerController;
class UMainMenuWidget;

UCLASS()
class MYPROJECT2_API AMainMenuGamemode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	AMainMenuGamemode();

protected:
	virtual void BeginPlay() override;

private:
	AAircraftPlayerController* APC;

	ACurrentPlayerState* PS;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UMainMenuWidget> MainMenuClass;
};
