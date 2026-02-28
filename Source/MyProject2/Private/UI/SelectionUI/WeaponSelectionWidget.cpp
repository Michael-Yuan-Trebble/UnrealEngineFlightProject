// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("WeaponSelect!"));
#include "UI/SelectionUI/WeaponSelectionWidget.h"
#include "Components/ScrollBox.h"
#include "Kismet/GameplayStatics.h"
#include "UI/SelectionUI/WeaponSelectionComponent.h"
#include "UI/SelectionUI/WeaponButtonWidget.h"

UWeaponSelectionWidget::UWeaponSelectionWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UWeaponSelectionWidget::GetAllAircraft() 
{
    if (!WeaponButtonClass || !WeaponScrollBox) return;

    WeaponScrollBox->ClearChildren();

    // Create a button for no equip
    UWeaponButtonWidget* NoneButton = CreateWidget<UWeaponButtonWidget>(GetWorld(), WeaponButtonClass);
    NoneButton->SetupWeapons(nullptr);
    NoneButton->OnWeaponSelected.AddDynamic(this, &UWeaponSelectionWidget::HandleWeaponSelected);
    if (IsValid(WeaponUI)) NoneButton->OnWeaponPicked.AddDynamic(WeaponUI, &UWeaponSelectionComponent::AddWeapon);
    WeaponScrollBox->AddChild(NoneButton);

    CreateButtons(CurrentLoadout.AllowedMissiles);
    CreateButtons(CurrentLoadout.AllowedBombs);
    CreateButtons(CurrentLoadout.AllowedMisc);
}

void UWeaponSelectionWidget::CreateButtons(const TArray<TSubclassOf<ABaseWeapon>>&Array) 
{
    if (Array.Num() <= 0) return;
    for (TSubclassOf<ABaseWeapon> SingleWeapon : Array)
    {
        if (!SingleWeapon) continue;

        UWeaponButtonWidget* Card = CreateWidget<UWeaponButtonWidget>(GetWorld(), WeaponButtonClass);
        if (!Card) return;

        FString WeaponName = SingleWeapon->GetName();

        Card->SetupWeapons(SingleWeapon);
        Card->OnWeaponSelected.AddDynamic(this, &UWeaponSelectionWidget::HandleWeaponSelected);
        Card->OnWeaponPicked.AddDynamic(WeaponUI, &UWeaponSelectionComponent::AddWeapon);
        WeaponScrollBox->AddChild(Card);
    } 
}

void UWeaponSelectionWidget::HandleWeaponSelected(TSubclassOf<ABaseWeapon> Weapon)
{
    OnWeaponSelected.Broadcast(Weapon);
}