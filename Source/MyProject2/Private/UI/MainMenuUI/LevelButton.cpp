// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MainMenuUI/LevelButton.h"

void ULevelButton::SetupLevel(const FMissionData& InLevel) 
{
	if (InLevel.Level.IsNull()) return;
	MissionData = InLevel;

	LevelText->SetText(FText::FromString(MissionData.Level.GetAssetName()));
}

void ULevelButton::NativeConstruct() 
{
	Super::NativeConstruct();
	if (!LevelSelectbtn) return;

	LevelSelectbtn->OnClicked.AddDynamic(this, &ULevelButton::HandleButtonClick);
}

void ULevelButton::HandleButtonClick() 
{
	if (MissionData.Level.IsNull()) return;
	OnLevelPicked.Broadcast(MissionData);
}