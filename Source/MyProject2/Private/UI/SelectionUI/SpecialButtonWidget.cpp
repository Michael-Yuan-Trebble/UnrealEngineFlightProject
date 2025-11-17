// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SelectionUI/SpecialButtonWidget.h"
#include "Specials/BaseSpecial.h"

void USpecialButtonWidget::Setup(TSubclassOf<UBaseSpecial> SpecialData)
{
	ContainedData = SpecialData;
	const UBaseSpecial* DefaultSpecial = ContainedData->GetDefaultObject<UBaseSpecial>();
	SpecialNameText->SetText(FText::FromName(DefaultSpecial->SpecialName));
	if (!SpecialSelectButton) return;
	SpecialSelectButton->OnClicked.AddDynamic(this, &USpecialButtonWidget::HandleButtonClick);
}

void USpecialButtonWidget::HandleButtonClick() 
{
	if (!ContainedData) return;
	OnSpecialPicked.Broadcast(ContainedData);
}