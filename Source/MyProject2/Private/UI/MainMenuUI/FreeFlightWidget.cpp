// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/MainMenuUI/FreeFlightWidget.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"

void UFreeFlightWidget::InitLevels()
{
	for (const FLevelInfo Level : LevelList->Levels) 
	{
		if (Level.LevelData.Level.IsNull()) continue;

		ULevelButton* Button = CreateWidget<ULevelButton>(GetWorld(), LevelButtonClass);
		if (!IsValid(Button)) continue;

		Button->SetupLevel(Level.LevelData);
		Button->OnLevelPicked.AddDynamic(this, &UFreeFlightWidget::HandleLevelButtonClicked);
		if (!IsValid(LevelScrollBox)) continue;
		LevelScrollBox->AddChild(Button);
	}
}

void UFreeFlightWidget::HandleLevelButtonClicked(const FMissionData& InLevel)
{
	if (InLevel.Level.IsNull()) return;
	OnLevelSelected.Broadcast(InLevel);
}