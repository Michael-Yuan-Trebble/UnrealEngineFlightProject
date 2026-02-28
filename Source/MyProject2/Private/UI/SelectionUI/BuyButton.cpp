// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SelectionUI/BuyButton.h"
#include "Components/Button.h"

void UBuyButton::NativeConstruct() 
{
	Super::NativeConstruct();
	if (!IsValid(AircraftBuyButton)) return;
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
	AircraftBuyButton->SetIsEnabled(false);
	FLinearColor Tint = FLinearColor::Gray;
	AircraftBuyButton->SetBackgroundColor(Tint);
}

void UBuyButton::HandleButtonClick() 
{
	OnBuyPicked.Broadcast(Name, Cost);
}

void UBuyButton::HandleCancelClick() 
{
	OnCancelPicked.Broadcast();
}