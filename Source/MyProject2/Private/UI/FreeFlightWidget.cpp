// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/FreeFlightWidget.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"

void UFreeFlightWidget::InitLevels()
{
	for (const FLevelInfo Level : LevelList->Levels) 
	{
		if (Level.LevelName.IsNone()) return;

		ULevelButton* Button = CreateWidget<ULevelButton>(GetWorld(), LevelButtonClass);
		if (!Button) return;

		Button->SetupLevel(Level.LevelName);
		Button->OnLevelPicked.AddDynamic(this, &UFreeFlightWidget::HandleLevelButtonClicked);
	}
}

void UFreeFlightWidget::HandleLevelButtonClicked(const FName InLevel) 
{
	if (InLevel.IsNone()) return;
	OnLevelSelected.Broadcast(InLevel);
}