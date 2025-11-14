// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MainMenuUI/MainMenuWidget.h"

UMainMenuWidget::UMainMenuWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) 
{

}

void UMainMenuWidget::CreateButtons() 
{
	Settingsbtn->OnClicked.AddDynamic(this, &UMainMenuWidget::HandleSettingsbtnClicked);
	FreeFlightbtn->OnClicked.AddDynamic(this, &UMainMenuWidget::HandleFreeFlightbtnClicked);
}

void UMainMenuWidget::HandleFreeFlightbtnClicked() 
{
	OnFreeFlightPicked.Broadcast();
}

void UMainMenuWidget::HandleSettingsbtnClicked() 
{
	OnSettingsPicked.Broadcast();
}