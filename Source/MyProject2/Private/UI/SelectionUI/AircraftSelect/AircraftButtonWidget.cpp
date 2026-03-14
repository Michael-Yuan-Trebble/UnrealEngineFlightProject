// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/SelectionUI/AircraftSelect/AircraftButtonWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Debug/DebugHelper.h"

void UAircraftButtonWidget::Setup(UAircraftData* AircraftData, bool bOwned)
{
	ContainedData = AircraftData;
	if (!IsValid(ContainedData) || !IsValid(AircraftNameText)) return;

	UAircraftStats* Loaded = ContainedData->AircraftStat.LoadSynchronous();
	if (!IsValid(Loaded)) return;

	AircraftNameText->SetText(FText::FromName(Loaded->AircraftName));

	if (IsValid(AircraftSelectButton)) {
		AircraftSelectButton->OnHovered.AddDynamic(this, &UAircraftButtonWidget::HandleButtonHover);

		if (bOwned) 
			AircraftSelectButton->OnClicked.AddDynamic(this, &UAircraftButtonWidget::HandleButtonClick);
		else 
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
	OnAircraftSelected.Broadcast(ContainedData);
}

void UAircraftButtonWidget::HandleButtonClick() 
{
	OnAircraftPicked.Broadcast(ContainedData);
}

void UAircraftButtonWidget::HandleBuyCreate() 
{
	OnBuyCreate.Broadcast(ContainedData, ContainedData->price);
}