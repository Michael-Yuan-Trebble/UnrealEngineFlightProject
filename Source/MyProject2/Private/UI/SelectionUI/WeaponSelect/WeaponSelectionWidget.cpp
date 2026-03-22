// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/SelectionUI/WeaponSelect/WeaponSelectionWidget.h"
#include "Components/ScrollBox.h"
#include "UI/SelectionUI/WeaponSelect/WeaponSelectionComponent.h"
#include "UI/SelectionUI/WeaponSelect/WeaponButtonWidget.h"

UWeaponSelectionWidget::UWeaponSelectionWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UWeaponSelectionWidget::GetAllAircraft() 
{
    if (!IsValid(WeaponButtonClass) || !IsValid(WeaponScrollBox)) return;

    WeaponScrollBox->ClearChildren();

    // Create a button for no equip
    NoneButton = CreateWidget<UWeaponButtonWidget>(GetWorld(), WeaponButtonClass);
    if (!IsValid(NoneButton)) return;
    NoneButton->SetupWeapons(nullptr);
    NoneButton->OnWeaponSelected.AddDynamic(this, &UWeaponSelectionWidget::HandleWeaponSelected);
    if (IsValid(WeaponUI)) NoneButton->OnWeaponPicked.AddDynamic(this, &UWeaponSelectionWidget::HandleWeaponAdded);
    WeaponScrollBox->AddChild(NoneButton);

    FPylonLoadout Loadouts = CurrentLoadout.PylonLoadout;

    CreateButtons(Loadouts.AllowedMissiles);
    CreateButtons(Loadouts.AllowedBombs);
    CreateButtons(Loadouts.AllowedMisc);
}

void UWeaponSelectionWidget::CreateButtons(const TArray<TSubclassOf<ABaseWeapon>>&Array) 
{
    if (Array.Num() <= 0 || !IsValid(WeaponScrollBox) || !IsValid(WeaponButtonClass)) return;
    for (TSubclassOf<ABaseWeapon> SingleWeapon : Array)
    {
        if (!IsValid(SingleWeapon)) continue;

        UWeaponButtonWidget* Card = CreateWidget<UWeaponButtonWidget>(GetWorld(), WeaponButtonClass);
        if (!IsValid(Card)) return;

        Cards.Add(Card);

        FString WeaponName = SingleWeapon->GetName();

        Card->SetupWeapons(SingleWeapon);
        Card->OnWeaponSelected.AddDynamic(this, &UWeaponSelectionWidget::HandleWeaponSelected);
        Card->OnWeaponPicked.AddDynamic(this, &UWeaponSelectionWidget::HandleWeaponAdded);
        WeaponScrollBox->AddChild(Card);
    } 
}

void UWeaponSelectionWidget::HandleWeaponSelected(TSubclassOf<ABaseWeapon> Weapon)
{
    OnWeaponSelected.Broadcast(Weapon, CurrentLoadout);
}

void UWeaponSelectionWidget::HandleWeaponAdded(TSubclassOf<ABaseWeapon> Weapon) {
    WeaponAdded.Broadcast(Weapon, CurrentLoadout);
}

void UWeaponSelectionWidget::NativeDestruct() {
    OnWeaponSelected.Clear();
    for (UWeaponButtonWidget* Card : Cards) {
        Card->OnWeaponSelected.RemoveAll(this);
        Card->OnWeaponPicked.RemoveAll(this);
    }
    if (IsValid(NoneButton)) {
        NoneButton->OnWeaponSelected.RemoveAll(this);
        NoneButton->OnWeaponPicked.RemoveAll(this);
    }
    Super::NativeDestruct();
}