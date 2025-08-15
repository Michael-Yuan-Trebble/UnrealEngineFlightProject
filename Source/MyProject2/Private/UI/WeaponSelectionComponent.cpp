// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WeaponSelectionComponent.h"
#include "UI/WeaponSelectionWidget.h"
#include "SaveGameManager.h"
#include "Gamemodes/PlayerGameInstance.h"
#include "MenuManagerComponent.h"
#include "Gamemodes/AircraftSelectionGamemode.h"
#include "AircraftPlayerController.h"

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
	CurrentPylonIndex = 0;
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
	if (!Weapon || !PC) return;
	FString PylonString = FString::Printf(TEXT("Pylon%d"), CurrentPylonIndex);
	FName PylonName = FName(*PylonString);
	GM->SpawnInWeapon(Weapon, PylonName);
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
	if (CurrentPylonIndex + 1 >= Aircraft->NumOfPylons)
	{
		MenuManager->ChooseSpecialUI();
	}
	else
	{
		CurrentPylonIndex++;
		PC->ManageMenuSetting(EMenuState::WeaponSelect);
	}
}

void UWeaponSelectionComponent::CloseAll()
{
	if (IsValid(WeaponSelectUI))
	{
		if (WeaponSelectUI->IsInViewport())
		{
			WeaponSelectUI->RemoveFromParent();
		}
		WeaponSelectUI = nullptr;
	}
}