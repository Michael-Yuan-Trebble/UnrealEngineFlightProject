// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MainMenuUI/LevelButton.h"

void ULevelButton::SetupLevel(FName InLevel) 
{
	if (InLevel.IsNone()) return;
	LevelName = InLevel;

	LevelText->SetText(FText::FromName(LevelName));
}

void ULevelButton::NativeConstruct() 
{
	Super::NativeConstruct();
	if (!LevelSelectbtn) return;

	LevelSelectbtn->OnClicked.AddDynamic(this, &ULevelButton::HandleButtonClick);
}

void ULevelButton::HandleButtonClick() 
{
	if (LevelName.IsNone()) return;
	OnLevelPicked.Broadcast(LevelName);
}