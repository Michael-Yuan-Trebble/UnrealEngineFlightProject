// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/SelectionUI/AircraftButtonWidget.h"
#include "Gamemodes/AircraftSelectionGamemode.h"
#include "Kismet/GameplayStatics.h"

void UAircraftButtonWidget::Setup(UAircraftData* AircraftData, TArray<FName> TempOwned)
{
	ContainedData = AircraftData;
	Owned = TempOwned;
	if (!IsValid(ContainedData) || ContainedData->AircraftStat.IsValid()) return;

	UAircraftStats* Loaded = ContainedData->AircraftStat.LoadSynchronous();
	if (!IsValid(Loaded)) return;

	AircraftNameText->SetText(FText::FromName(Loaded->AircraftName));

	if (!IsValid(AircraftSelectButton)) return;

	AircraftSelectButton->OnHovered.AddDynamic(this, &UAircraftButtonWidget::HandleButtonHover);
	if (Owned.Contains(Loaded->AircraftName))
	{
		AircraftSelectButton->OnClicked.AddDynamic(this, &UAircraftButtonWidget::HandleButtonClick);
	}
	else 
	{
		AircraftSelectButton->OnClicked.AddDynamic(this, &UAircraftButtonWidget::HandleBuyCreate);
	}
}

void UAircraftButtonWidget::AdjustButtons() 
{
	AircraftSelectButton->OnClicked.RemoveDynamic(this, &UAircraftButtonWidget::HandleBuyCreate);
	AircraftSelectButton->OnClicked.AddDynamic(this, &UAircraftButtonWidget::HandleButtonClick);
}

void UAircraftButtonWidget::HandleButtonHover() 
{
	if (!IsValid(ContainedData->AircraftClass)) return;
	
	OnAircraftSelected.Broadcast(ContainedData);
}

void UAircraftButtonWidget::HandleButtonClick() 
{
	if (!IsValid(ContainedData->AircraftClass)) return;
	
	OnAircraftPicked.Broadcast(ContainedData);
}

void UAircraftButtonWidget::HandleBuyCreate() 
{
	if (!IsValid(ContainedData)) return;
	OnBuyCreate.Broadcast(ContainedData, ContainedData->price);
}