// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("WeaponSelect!"));
#include "UI/SelectionUI/WeaponButtonWidget.h"
#include "Gamemodes/AircraftSelectionGamemode.h"
#include "Weapons/BaseWeapon.h"
#include "Kismet/GameplayStatics.h"

void UWeaponButtonWidget::SetupWeapons(const TSubclassOf<ABaseWeapon>& WeaponData)
{
	ButtonWeapon = WeaponData;
	if (!WeaponData) 
	{
		WeaponNameText->SetText(FText::FromString("None"));
		return;
	}

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
	OnWeaponPicked.Broadcast(ButtonWeapon);
}

void UWeaponButtonWidget::HandleButtonHover() 
{
	OnWeaponSelected.Broadcast(ButtonWeapon);
}