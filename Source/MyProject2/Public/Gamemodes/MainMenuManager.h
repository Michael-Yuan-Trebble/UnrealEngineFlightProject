// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Structs and Data/LevelListData.h"
#include "Structs and Data/MissionData.h"
#include "MainMenuManager.generated.h"

class UMainMenuWidget;
class UFreeFlightWidget;
class AAircraftPlayerController;
class UPlayerGameInstance;

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
	void OnLevelPicked(FMissionData LevelName);

	bool OpenLevel(const FName& LevelName);

	UPROPERTY()
	AAircraftPlayerController* APC = nullptr;

	UPROPERTY()
	TSubclassOf<UMainMenuWidget> MainMenuClass = nullptr;

	UPROPERTY()
	TSubclassOf<UFreeFlightWidget> FreeFlightClass = nullptr;

	UPROPERTY()
	TSubclassOf<UUserWidget> TransitionScreenClass = nullptr;

	UPROPERTY()
	UPlayerGameInstance* PlayerInstance = nullptr;

private:
	UPROPERTY()
	UMainMenuWidget* MainMenuWidget = nullptr;

	UPROPERTY()
	UFreeFlightWidget* FreeFlightWidget = nullptr;

	UPROPERTY()
	UUserWidget* TransitionScreenWidget = nullptr;

	UPROPERTY()
	TArray<UUserWidget*> MenuStack;

	UPROPERTY()
	UUserWidget* CurrentMenu = nullptr;
};
