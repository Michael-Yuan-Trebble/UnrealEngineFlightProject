// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Main Menu Manager!"));
#include "Gamemodes/MainMenuManager.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "UI/MainMenuUI/FreeFlightWidget.h"
#include "Player Info/PlayerGameInstance.h"
#include "UI/MainMenuUI/MainMenuWidget.h"
#include "Gamemodes/MainMenuGamemode.h"

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
	if (!MainMenuClass) return;
	if (!MainMenuClass->IsChildOf(UMainMenuWidget::StaticClass())) return;

	if (!MainMenuWidget) 
	{
		MainMenuWidget = CreateWidget<UMainMenuWidget>(GetWorld(), MainMenuClass);
		if (!MainMenuWidget) return;
		if (!MainMenuWidget->IsA(UMainMenuWidget::StaticClass())) return;
		MainMenuWidget->CreateButtons();
		MainMenuWidget->OnSettingsPicked.AddDynamic(this, &UMainMenuManager::ShowSettings);
		MainMenuWidget->OnFreeFlightPicked.AddDynamic(this, &UMainMenuManager::ShowFreeFlight);
		MainMenuWidget->AddToViewport(0);
	}

	if (CurrentMenu) CurrentMenu->SetVisibility(ESlateVisibility::Hidden);

	MainMenuWidget->SetVisibility(ESlateVisibility::Visible);
	CurrentMenu = MainMenuWidget;

	MenuStack.Push(CurrentMenu);
}

void UMainMenuManager::ShowFreeFlight() 
{
	if (!FreeFlightClass) return;
	if (!FreeFlightClass->IsChildOf(UFreeFlightWidget::StaticClass())) return;

	if (!FreeFlightWidget) 
	{
		FreeFlightWidget = CreateWidget<UFreeFlightWidget>(GetWorld(), FreeFlightClass);
		if (!FreeFlightWidget) return;
		if (!FreeFlightWidget->IsA(UFreeFlightWidget::StaticClass())) return;
		FreeFlightWidget->InitLevels();
		FreeFlightWidget->OnLevelSelected.AddDynamic(this, &UMainMenuManager::OnLevelPicked);
		FreeFlightWidget->AddToViewport(1);
	}

	if (CurrentMenu) CurrentMenu->SetVisibility(ESlateVisibility::Hidden);

	CurrentMenu = FreeFlightWidget;

	MenuStack.Push(FreeFlightWidget);
}

void UMainMenuManager::OnLevelPicked(FMissionData LevelName)
{
	if (TransitionScreenClass)
	{
		TransitionScreenWidget = CreateWidget<UUserWidget>(GetWorld(), TransitionScreenClass);
		if (TransitionScreenWidget) 
		{
			TransitionScreenWidget->AddToViewport(1000);
		}
	}

	AMainMenuGamemode* GM = Cast<AMainMenuGamemode>(GetWorld()->GetAuthGameMode());
	if (!IsValid(GM)) return;
	GM->LevelSelected(LevelName);
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
	for (UUserWidget* Menu : MenuStack) 
	{
		if (Menu && Menu->IsInViewport()) 
		{
			Menu->RemoveFromParent();
		}
	}
	if (bClear) 
	{
		MenuStack.Empty();
	}
	CurrentMenu = nullptr;
}