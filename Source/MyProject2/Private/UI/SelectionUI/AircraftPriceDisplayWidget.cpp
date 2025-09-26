// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SelectionUI/AircraftPriceDisplayWidget.h"

void UAircraftPriceDisplayWidget::DisplayAircraftPriceInfo(UAircraftData* Aircraft)
{
	if (!Aircraft) return;
	AircraftPriceText->SetText(FText::AsNumber(Aircraft->price));
	AircraftNameText->SetText(FText::FromName(Aircraft->AircraftStat->AircraftName));
}