// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SelectionUI/WeaponSelectionComponent.h"
#include "UI/SelectionUI/WeaponSelectionWidget.h"
#include "Player Info/SaveGameManager.h"
#include "Player Info/PlayerGameInstance.h"
#include "Units/Aircraft/MenuManagerComponent.h"
#include "Gamemodes/AircraftSelectionGamemode.h"
#include "Player Info/AircraftPlayerController.h"

UWeaponSelectionComponent::UWeaponSelectionComponent()
{
	static ConstructorHelpers::FClassFinder<UUserWidget> WeaponBPClass(TEXT("/Game/UI/BPWeaponSelectWidget"));
	if (WeaponBPClass.Succeeded())
	{
		SelectionWidget = WeaponBPClass.Class;
	}
}

void UWeaponSelectionComponent::Setup(AAircraftPlayerController* InPlayer, AAircraftSelectionGamemode* InGM, ACurrentPlayerState* InPS, UMenuManagerComponent* InMenu)
{
	PC = InPlayer;
	GM = InGM;
	PS = InPS;
	MenuManager = InMenu;
}

void UWeaponSelectionComponent::AddAircraft(UAircraftData* Data)
{
	Aircraft = Data;
}

void UWeaponSelectionComponent::WeaponSelectionMenu() 
{
	if (PC == nullptr) return;

	WeaponSelectUI = CreateWidget<UWeaponSelectionWidget>(PC, SelectionWidget);
	if (!WeaponSelectUI) return;

	FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputMode.SetHideCursorDuringCapture(true);

	// TODO: Make it so the structs contain how many pylons are in a group, like 2, and then just loop twice for the pylons

	WeaponSelectUI->CurrentLoadout = &(Aircraft->PylonLoadouts[CurrentPylonIndex]);
	WeaponSelectUI->WeaponUI = this;
	WeaponSelectUI->GetAllAircraft();
	WeaponSelectUI->AddToViewport();
	InputMode.SetWidgetToFocus(WeaponSelectUI->TakeWidget());
	PC->SetInputMode(InputMode);
	PC->bShowMouseCursor = true;
	WeaponSelectUI->OnWeaponSelected.AddDynamic(this, &UWeaponSelectionComponent::HandleWeaponPicked);
}

void UWeaponSelectionComponent::HandleWeaponPicked(TSubclassOf<ABaseWeapon> Weapon)
{
	if (!PC || !GM) return;
	FString PylonString = FString::Printf(TEXT("Pylon%d"), CurrentPylonIndex);
	FName PylonName = FName(*PylonString);
	if (!Weapon)
	{
		GM->ClearWeapons(PylonName);
	}
	else 
	{
		GM->SpawnInWeapon(Weapon, PylonName);
	}
}

void UWeaponSelectionComponent::AddWeapon(TSubclassOf<ABaseWeapon> Weapon) 
{
	FString PylonString = FString::Printf(TEXT("Pylon%d"), CurrentPylonIndex);
	FName PylonName = FName(*PylonString);
	WeaponSelection.Add(PylonName, Weapon);
	CheckWeaponLoop();
}

void UWeaponSelectionComponent::CheckWeaponLoop() 
{
	CurrentPylonIndex++;
	if (CurrentPylonIndex >= Aircraft->NumOfPylons)
	{
		MenuManager->ChooseSpecialUI();
	}
	else
	{
		PC->ManageMenuSetting(EMenuState::WeaponSelect);
	}
}

void UWeaponSelectionComponent::CloseAll()
{
	if (IsValid(WeaponSelectUI))
	{
		WeaponSelectUI->OnWeaponSelected.RemoveAll(this);
		if (WeaponSelectUI->IsInViewport())
		{
			WeaponSelectUI->RemoveFromParent();
		}
		WeaponSelectUI = nullptr;
	}
}