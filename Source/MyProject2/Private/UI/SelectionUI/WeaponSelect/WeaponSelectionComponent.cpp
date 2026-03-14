// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SelectionUI/WeaponSelect/WeaponSelectionComponent.h"
#include "UI/SelectionUI/WeaponSelect/WeaponSelectionWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Units/Components/Player/MenuManagerComponent.h"
#include "Gamemodes/AircraftSelectionGamemode.h"
#include "Player Info/AircraftPlayerController.h"

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
	if (Aircraft->PylonLoadouts.Num() > 0) {
		WeaponSelectUI->SetLoadout(Aircraft->PylonLoadouts[CurrentPylonIndex]);
	}

	WeaponSelectUI->SetWeaponUI(this);
	WeaponSelectUI->GetAllAircraft();
	WeaponSelectUI->AddToViewport();
	InputMode.SetWidgetToFocus(WeaponSelectUI->TakeWidget());
	PC->SetInputMode(InputMode);
	PC->bShowMouseCursor = true;
	WeaponSelectUI->OnWeaponSelected.AddDynamic(this, &UWeaponSelectionComponent::HandleWeaponPicked);
}

void UWeaponSelectionComponent::HandleWeaponPicked(TSubclassOf<ABaseWeapon> Weapon)
{
	AAircraftSelectionGamemode* GM = Cast<AAircraftSelectionGamemode>(UGameplayStatics::GetGameMode(this));
	if (!IsValid(GM)) return;

	FString PylonString = FString::Printf(TEXT("Pylon_%d"), CurrentPylonIndex);
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
	FString PylonString = FString::Printf(TEXT("Pylon_%d"), CurrentPylonIndex);
	FName PylonName = FName(*PylonString);
	WeaponSelection.Add(PylonName, Weapon);
	CheckWeaponLoop();
}

void UWeaponSelectionComponent::CheckWeaponLoop() 
{
	if (!IsValid(GetWorld())) return;

	AAircraftPlayerController* PC = Cast<AAircraftPlayerController>(GetWorld()->GetFirstPlayerController());
	if (!IsValid(PC)) return;

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

UUserWidget* UWeaponSelectionComponent::GetWeaponUI() const { 
	return WeaponSelectUI; 
}