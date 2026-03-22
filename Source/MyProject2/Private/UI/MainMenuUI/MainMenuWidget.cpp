// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/MainMenuUI/MainMenuWidget.h"
#include "Components/Button.h"

UMainMenuWidget::UMainMenuWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) 
{

}

void UMainMenuWidget::CreateButtons() 
{
	if (!IsValid(Settingsbtn) || !IsValid(FreeFlightbtn)) return;
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

void UMainMenuWidget::NativeDestruct() {
	if (IsValid(Settingsbtn))
		Settingsbtn->OnClicked.RemoveAll(this);
	if (IsValid(FreeFlightbtn))
		FreeFlightbtn->OnClicked.RemoveAll(this);
}