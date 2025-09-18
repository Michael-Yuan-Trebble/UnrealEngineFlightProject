// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Button!"));
#include "UI/SelectionUI/AircraftButtonWidget.h"
#include "Gamemodes/AircraftSelectionGamemode.h"
#include "Kismet/GameplayStatics.h"

void UAircraftButtonWidget::Setup(UAircraftData* AircraftData, TArray<FName> TempOwned)
{
	ContainedData = AircraftData;
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

void UAircraftButtonWidget::AdjustButtons() 
{
	AircraftSelectButton->OnClicked.RemoveDynamic(this, &UAircraftButtonWidget::HandleBuyCreate);
	AircraftSelectButton->OnClicked.AddDynamic(this, &UAircraftButtonWidget::HandleButtonClick);
}

void UAircraftButtonWidget::HandleButtonHover() 
{
	if (!(ContainedData->AircraftClass)) return;
	
	OnAircraftSelected.Broadcast(ContainedData);
}

void UAircraftButtonWidget::HandleButtonClick() 
{
	if (!(ContainedData->AircraftClass)) return;
	
	OnAircraftPicked.Broadcast(ContainedData);
}

void UAircraftButtonWidget::HandleBuyCreate() 
{
	if (!ContainedData) return;
	OnBuyCreate.Broadcast(ContainedData, ContainedData->price);
}