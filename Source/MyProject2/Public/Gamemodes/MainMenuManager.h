// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MainMenuManager.generated.h"

class UMainMenuWidget;
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
	void ShowSettings();
	void ShowFreeFlight();


	// Other Menu Functions

	void GoBack();

	void HideAll(bool bClear);

	void Init(AAircraftPlayerController* InAPC);

	AAircraftPlayerController* APC;

private:
	UMainMenuWidget* MainMenuWidget;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UMainMenuWidget> MainMenuClass;

	TArray<UUserWidget*> MenuStack;

	UUserWidget* CurrentMenu;
};
