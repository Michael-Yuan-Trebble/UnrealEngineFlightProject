// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SelectionUI/Buy/BuyButton.h"
#include "Components/Button.h"

void UBuyButton::NativeConstruct() 
{
	Super::NativeConstruct();
	if (!IsValid(AircraftBuyButton) || !IsValid(CancelBuyButton)) return;
	AircraftBuyButton->OnClicked.AddDynamic(this, &UBuyButton::HandleButtonClick);
	CancelBuyButton->OnClicked.AddDynamic(this, &UBuyButton::HandleCancelClick);
}

void UBuyButton::Setup(const FName& InName, const int InCost)
{
	Name = InName;
	Cost = InCost;
}

void UBuyButton::TurnOffBuy() 
{
	if (!IsValid(AircraftBuyButton)) return;
	AircraftBuyButton->SetIsEnabled(false);
	FLinearColor Tint = FLinearColor::Gray;
	AircraftBuyButton->SetBackgroundColor(Tint);
}

void UBuyButton::HandleButtonClick() 
{
	OnBuyPressed.Broadcast(true);
}

void UBuyButton::HandleCancelClick() 
{
	OnBuyPressed.Broadcast(false);
}