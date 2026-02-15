// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MainMenuUI/LevelButton.h"

void ULevelButton::SetupLevel(const FMissionData& InLevel) 
{
	if (InLevel.LevelName.IsNone()) return;
	MissionData = InLevel;

	LevelText->SetText(FText::FromName(MissionData.LevelName));
}

void ULevelButton::NativeConstruct() 
{
	Super::NativeConstruct();
	if (!LevelSelectbtn) return;

	LevelSelectbtn->OnClicked.AddDynamic(this, &ULevelButton::HandleButtonClick);
}

void ULevelButton::HandleButtonClick() 
{
	if (MissionData.LevelName.IsNone()) return;
	OnLevelPicked.Broadcast(MissionData);
}