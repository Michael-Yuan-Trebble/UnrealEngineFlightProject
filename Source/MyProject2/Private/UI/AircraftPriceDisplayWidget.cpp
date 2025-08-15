// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/AircraftPriceDisplayWidget.h"

void UAircraftPriceDisplayWidget::DisplayAircraftPriceInfo(UAircraftData* Aircraft)
{
	if (!Aircraft) return;
	AircraftPriceText->SetText(FText::AsNumber(Aircraft->price));
	AircraftNameText->SetText(FText::FromName(Aircraft->AircraftName));
}