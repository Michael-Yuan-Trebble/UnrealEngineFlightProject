// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/AircraftButtonWidget.h"
#include "Gamemodes/AircraftSelectionGamemode.h"
#include "Kismet/GameplayStatics.h"

void UAircraftButtonWidget::Setup(UAircraftData* AircraftData, int Money, TArray<FName> TempOwned)
{
	ContainedData = AircraftData;
	CurrentMoney = Money;
	Owned = TempOwned;
	AircraftNameText->SetText(FText::FromName(ContainedData->AircraftName));

	if (!AircraftSelectButton) return;

	AircraftSelectButton->OnHovered.AddDynamic(this, &UAircraftButtonWidget::HandleButtonHover);
	if (Owned.Contains(AircraftData->AircraftName))
	{
		AircraftSelectButton->OnClicked.AddDynamic(this, &UAircraftButtonWidget::HandleButtonClick);
	}
	else 
	{
		AircraftSelectButton->OnClicked.AddDynamic(this, &UAircraftButtonWidget::HandleBuyCreate);
	}
}

void UAircraftButtonWidget::HandleButtonHover() 
{
	if (!(ContainedData->AircraftClass)) return;
	
	OnAircraftSelected.Broadcast(ContainedData->AircraftClass);
}

void UAircraftButtonWidget::HandleButtonClick() 
{
	if (!(ContainedData->AircraftClass)) return;
	
	OnAircraftPicked.Broadcast(ContainedData);
}

void UAircraftButtonWidget::HandleBuyCreate() {
	if (!ContainedData) return;
	OnBuyCreate.Broadcast(ContainedData->AircraftName, CurrentMoney);
}