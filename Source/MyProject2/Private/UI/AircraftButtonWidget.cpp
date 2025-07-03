// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/AircraftButtonWidget.h"
#include "Gamemodes/AircraftSelectionGamemode.h"
#include "Kismet/GameplayStatics.h"

void UAircraftButtonWidget::Setup(UAircraftData* AircraftData) 
{
	ContainedData = AircraftData;
	AircraftNameText->SetText(ContainedData->AircraftName);
}

void UAircraftButtonWidget::NativeConstruct() 
{
	Super::NativeConstruct();
	if (AircraftSelectButton)
	{
		AircraftSelectButton->OnHovered.AddDynamic(this, &UAircraftButtonWidget::HandleButtonHover);
		AircraftSelectButton->OnClicked.AddDynamic(this, &UAircraftButtonWidget::HandleButtonClick);
	}
}

void UAircraftButtonWidget::HandleButtonHover() 
{
	if (ContainedData->AircraftClass) 
	{
		OnAircraftSelected.Broadcast(ContainedData->AircraftClass);
	}
}

void UAircraftButtonWidget::HandleButtonClick() 
{
	if (ContainedData->AircraftClass) 
	{
		OnAircraftPicked.Broadcast(ContainedData);
	}
}