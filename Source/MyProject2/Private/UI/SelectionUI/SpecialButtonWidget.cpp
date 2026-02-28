// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SelectionUI/SpecialButtonWidget.h"
#include "Specials/BaseSpecial.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

void USpecialButtonWidget::Setup(const TSubclassOf<UBaseSpecial>& SpecialData)
{
	ContainedData = SpecialData;
	if (!IsValid(ContainedData)) return;
	const UBaseSpecial* DefaultSpecial = ContainedData->GetDefaultObject<UBaseSpecial>();
	SpecialNameText->SetText(FText::FromName(DefaultSpecial->SpecialName));
	if (!IsValid(SpecialSelectButton)) return;
	SpecialSelectButton->OnClicked.AddDynamic(this, &USpecialButtonWidget::HandleButtonClick);
}

void USpecialButtonWidget::HandleButtonClick() 
{
	if (IsValid(ContainedData)) OnSpecialPicked.Broadcast(ContainedData);
}