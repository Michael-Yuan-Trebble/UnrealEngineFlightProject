// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Structs and Data/LevelListData.h"
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
	void OnLevelPicked(FName LevelName);

	bool OpenLevel(FName LevelName);

	UPROPERTY()
	AAircraftPlayerController* APC;

	UPROPERTY()
	TSubclassOf<UMainMenuWidget> MainMenuClass;

	UPROPERTY()
	TSubclassOf<UFreeFlightWidget> FreeFlightClass;

	UPROPERTY()
	TSubclassOf<UUserWidget> TransitionScreenClass;

	UPROPERTY()
	UPlayerGameInstance* PlayerInstance;

private:
	UPROPERTY()
	UMainMenuWidget* MainMenuWidget;

	UPROPERTY()
	UFreeFlightWidget* FreeFlightWidget;

	UPROPERTY()
	UUserWidget* TransitionScreenWidget;

	UPROPERTY()
	TArray<UUserWidget*> MenuStack;

	UPROPERTY()
	UUserWidget* CurrentMenu;
};
