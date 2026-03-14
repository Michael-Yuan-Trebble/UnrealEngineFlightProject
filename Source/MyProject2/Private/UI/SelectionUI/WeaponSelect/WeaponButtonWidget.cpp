// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/SelectionUI/WeaponSelect/WeaponButtonWidget.h"
#include "Gamemodes/AircraftSelectionGamemode.h"
#include "Weapons/BaseWeapon.h"
#include "Kismet/GameplayStatics.h"

void UWeaponButtonWidget::SetupWeapons(const TSubclassOf<ABaseWeapon>& WeaponData)
{
	if (!IsValid(WeaponNameText) || !IsValid(WeaponSelectButton)) return;
	ButtonWeapon = WeaponData;
	if (!IsValid(ButtonWeapon))
	{
		WeaponNameText->SetText(FText::FromString("None"));
		return;
	}

	const ABaseWeapon* DefaultWeapon = ButtonWeapon->GetDefaultObject<ABaseWeapon>();
	FName WeaponName = DefaultWeapon->GetName();
	WeaponNameText->SetText(FText::FromName(WeaponName));

	WeaponSelectButton->OnHovered.AddDynamic(this, &UWeaponButtonWidget::HandleButtonHover);
	WeaponSelectButton->OnClicked.AddDynamic(this, &UWeaponButtonWidget::HandleButtonClick);
}

void UWeaponButtonWidget::HandleButtonClick() 
{
	OnWeaponPicked.Broadcast(ButtonWeapon);
}

void UWeaponButtonWidget::HandleButtonHover() 
{
	OnWeaponSelected.Broadcast(ButtonWeapon);
}