// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/BuyButton.h"

void UBuyButton::NativeConstruct() {
	Super::NativeConstruct();
	if (!AircraftBuyButton) return;
	AircraftBuyButton->OnClicked.AddDynamic(this, &UBuyButton::HandleButtonClick);
	CancelBuyButton->OnClicked.AddDynamic(this, &UBuyButton::HandleCancelClick);
}

void UBuyButton::Setup(FName InName, int InCost) {
	Name = InName;
	Cost = InCost;
}

void UBuyButton::HandleButtonClick() 
{
	OnBuyPicked.Broadcast(Name, Cost);
}

void UBuyButton::HandleCancelClick() 
{
	OnCancelPicked.Broadcast();
}