// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SelectionUI/WeaponSelect/WeaponSelectionComponent.h"
#include "UI/SelectionUI/WeaponSelect/WeaponSelectionWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Units/Components/Player/MenuManagerComponent.h"
#include "Gamemodes/AircraftSelectionGamemode.h"
#include "Player Info/AircraftPlayerController.h"
#include "Debug/DebugHelper.h"

UWeaponSelectionComponent::UWeaponSelectionComponent()
{
}

void UWeaponSelectionComponent::Setup(UMenuManagerComponent* InMenu)
{
	MenuManager = InMenu;
}

void UWeaponSelectionComponent::AddAircraft(UAircraftData* Data)
{
	Aircraft = Data;
}

void UWeaponSelectionComponent::WeaponSelectionMenu() 
{
	if (!IsValid(GetWorld())) return;

	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!IsValid(PC)) return;

	WeaponSelectUI = CreateWidget<UWeaponSelectionWidget>(PC, SelectionWidget);
	if (!IsValid(WeaponSelectUI)) return;

	FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputMode.SetHideCursorDuringCapture(true);

	// TODO: Make it so the structs contain how many pylons are in a group, like 2, and then just loop twice for the pylons
	if (!IsValid(Aircraft)) return;
	if (Aircraft->PylonLoadouts.IsValidIndex(CurrentPylonIndex)) {
		WeaponSelectUI->SetLoadout(Aircraft->PylonLoadouts[CurrentPylonIndex]);
	}

	WeaponSelectUI->SetWeaponUI(this);
	WeaponSelectUI->GetAllAircraft();
	WeaponSelectUI->AddToViewport();
	InputMode.SetWidgetToFocus(WeaponSelectUI->TakeWidget());
	PC->SetInputMode(InputMode);
	PC->bShowMouseCursor = true;
	WeaponSelectUI->OnWeaponSelected.AddDynamic(this, &UWeaponSelectionComponent::HandleWeaponPicked);
	WeaponSelectUI->WeaponAdded.AddDynamic(this, &UWeaponSelectionComponent::AddWeapon);
}

void UWeaponSelectionComponent::HandleWeaponPicked(TSubclassOf<ABaseWeapon> Weapon, FAircraftWeaponEquipInfo Equipment)
{
	AAircraftSelectionGamemode* GM = Cast<AAircraftSelectionGamemode>(UGameplayStatics::GetGameMode(this));
	if (!IsValid(GM)) return;

	for (const FName& Number : Equipment.Pylons) {
		FName PylonName(*FString("Pylon_") + Number.ToString());

		if (!Weapon)
		{
			GM->ClearWeapons(PylonName);
		}
		else
		{
			GM->SpawnInWeapon(Weapon, PylonName);
		}
	}
}

void UWeaponSelectionComponent::AddWeapon(TSubclassOf<ABaseWeapon> Weapon, FAircraftWeaponEquipInfo Equipment)
{
	for (const FName& Number : Equipment.Pylons) {
		FName PylonName(*FString("Pylon_") + Number.ToString());
		WeaponSelection.Add(PylonName, Weapon);
	}
	CheckWeaponLoop(Equipment.Pylons);
}

void UWeaponSelectionComponent::CheckWeaponLoop(const TArray<FName>& Pylons)
{
	if (!IsValid(GetWorld())) return;

	CurrentPylonIndex++;
	//DEBUG_TIME(100.f, "Pylon Number: %d", CurrentPylonIndex);
	if (CurrentPylonIndex >= Aircraft->NumOfPylons)
	{
		MenuManager->ChooseSpecialUI();
	}
	else
	{
		if (auto* PC = Cast<AAircraftPlayerController>(GetWorld()->GetFirstPlayerController())) {
			PC->ManageMenuSetting(EMenuState::WeaponSelect);
		}
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

UUserWidget* UWeaponSelectionComponent::GetWeaponUI() const { 
	return WeaponSelectUI; 
}