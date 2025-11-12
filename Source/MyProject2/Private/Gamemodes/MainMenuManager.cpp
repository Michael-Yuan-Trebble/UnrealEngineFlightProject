// Fill out your copyright notice in the Description page of Project Settings.


#include "Gamemodes/MainMenuManager.h"
#include "Blueprint/UserWidget.h"
#include "UI/MainMenuWidget.h"

UMainMenuManager::UMainMenuManager() 
{

}

void UMainMenuManager::Initialize(FSubsystemCollectionBase& Collection) 
{
	Super::Initialize(Collection);
}

void UMainMenuManager::Deinitialize() 
{
	HideAll(true);
	Super::Deinitialize();
}

void UMainMenuManager::Init(AAircraftPlayerController* InAPC) 
{
	APC = InAPC;
}

void UMainMenuManager::ShowMainMenu() 
{
	HideAll(false);
	if (!MainMenuWidget) 
	{
		MainMenuWidget = CreateWidget<UMainMenuWidget>(GetWorld(), MainMenuClass);
		MainMenuWidget->OnSettingsPicked.AddDynamic(this, &UMainMenuManager::ShowSettings);
		MainMenuWidget->OnFreeFlightPicked.AddDynamic(this, &UMainMenuManager::ShowFreeFlight);
	}
	MainMenuWidget->AddToViewport();
	CurrentMenu = MainMenuWidget;

	MenuStack.Push(CurrentMenu);
}

void UMainMenuManager::ShowFreeFlight() 
{

}

void UMainMenuManager::ShowSettings() 
{

}

void UMainMenuManager::GoBack() 
{
	if (CurrentMenu == MainMenuWidget) return;

	if (MenuStack.Num() > 0) 
	{
		UUserWidget* Pop = MenuStack.Pop();
		if (Pop && Pop->IsInViewport()) Pop->RemoveFromParent();
	}
	if (MenuStack.Num() > 0) 
	{
		CurrentMenu = MenuStack.Last();
		if (CurrentMenu && !CurrentMenu->IsInViewport()) CurrentMenu->AddToViewport();
	}
	else CurrentMenu = nullptr;
}

void UMainMenuManager::HideAll(bool bClear)
{
	for (auto* Menu : MenuStack) {
		if (Menu && Menu->IsInViewport()) {
			Menu->RemoveFromParent();
		}
	}
	if (bClear) 
	{
		MenuStack.Empty();
		CurrentMenu = nullptr;
	}
}