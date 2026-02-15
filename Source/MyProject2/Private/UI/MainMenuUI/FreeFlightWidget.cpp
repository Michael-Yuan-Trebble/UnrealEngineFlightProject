// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Free Flight Widget!"));
#include "UI/MainMenuUI/FreeFlightWidget.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"

void UFreeFlightWidget::InitLevels()
{
	for (const FLevelInfo Level : LevelList->Levels) 
	{
		if (Level.LevelData.LevelName.IsNone()) continue;

		ULevelButton* Button = CreateWidget<ULevelButton>(GetWorld(), LevelButtonClass);
		if (!Button) continue;

		Button->SetupLevel(Level.LevelData);
		Button->OnLevelPicked.AddDynamic(this, &UFreeFlightWidget::HandleLevelButtonClicked);
		if (!LevelScrollBox) continue;
		LevelScrollBox->AddChild(Button);
	}
}

void UFreeFlightWidget::HandleLevelButtonClicked(FMissionData InLevel)
{
	if (InLevel.LevelName.IsNone()) return;
	OnLevelSelected.Broadcast(InLevel);
}