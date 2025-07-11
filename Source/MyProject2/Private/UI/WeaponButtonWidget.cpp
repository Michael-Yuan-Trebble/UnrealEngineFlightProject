// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/WeaponButtonWidget.h"
#include "Gamemodes/AircraftSelectionGamemode.h"
#include "Weapons/BaseWeapon.h"
#include "Kismet/GameplayStatics.h"

void UWeaponButtonWidget::SetupWeapons(TSubclassOf<ABaseWeapon> WeaponData)
{
	ButtonWeapon = WeaponData;
	if (!WeaponData) return;

	const ABaseWeapon* DefaultWeapon = WeaponData->GetDefaultObject<ABaseWeapon>();
	FName WeaponName = DefaultWeapon->WeaponName;
	WeaponNameText->SetText(FText::FromName(WeaponName));
}

void UWeaponButtonWidget::NativeConstruct() 
{
	Super::NativeConstruct();
	if (!WeaponSelectButton) return;

	WeaponSelectButton->OnHovered.AddDynamic(this, &UWeaponButtonWidget::HandleButtonHover);
	WeaponSelectButton->OnClicked.AddDynamic(this, &UWeaponButtonWidget::HandleButtonClick);
}

void UWeaponButtonWidget::HandleButtonClick() 
{
	if (!ButtonWeapon) return;
	
	OnWeaponPicked.Broadcast(ButtonWeapon);
}

void UWeaponButtonWidget::HandleButtonHover() 
{
	if (!ButtonWeapon) return

	OnWeaponSelected.Broadcast(ButtonWeapon);
}