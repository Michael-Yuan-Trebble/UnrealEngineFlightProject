// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Structs and Data/LevelListData.h"
#include "MainMenuManager.generated.h"

class UMainMenuWidget;
class UFreeFlightWidget;
class AAircraftPlayerController;

UCLASS()
class MYPROJECT2_API UMainMenuManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:

	UMainMenuManager();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	// All Menus Are Here
	void ShowMainMenu();

	UFUNCTION()
	void ShowSettings();

	UFUNCTION()
	void ShowFreeFlight();

	// Other Menu Functions

	void GoBack();

	void HideAll(bool bClear);

	void Init(AAircraftPlayerController* InAPC);

	UFUNCTION()
	void OnLevelPicked(FName LevelName);

	AAircraftPlayerController* APC;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UMainMenuWidget> MainMenuClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UFreeFlightWidget> FreeFlightClass;

private:
	UMainMenuWidget* MainMenuWidget;

	UFreeFlightWidget* FreeFlightWidget;

	TArray<UUserWidget*> MenuStack;

	UUserWidget* CurrentMenu;
};
