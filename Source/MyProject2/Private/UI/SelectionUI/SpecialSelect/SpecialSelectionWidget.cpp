// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/SelectionUI/SpecialSelect/SpecialSelectionWidget.h"
#include "Structs and Data/Aircraft Data/AircraftDatabase.h"
#include "UI/SelectionUI/SpecialSelect/SpecialSelectionComponent.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/ScrollBox.h"
#include "UI/SelectionUI/SpecialSelect/SpecialButtonWidget.h"
#include "Debug/DebugHelper.h"

USpecialSelectionWidget::USpecialSelectionWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void USpecialSelectionWidget::Setup(UAircraftData* InAircraft, USpecialSelectionComponent* InSelect) 
{
	AircraftSelected = InAircraft;
	SpecialUI = InSelect;
}

void USpecialSelectionWidget::GetAllSpecials() 
{
	if (!IsValid(SpecialButtonClass) || !IsValid(SpecialScrollBox) || !IsValid(SpecialUI) || !IsValid(GetWorld())) return;
	SpecialScrollBox->ClearChildren();

	for (TSubclassOf<UBaseSpecial> Data : AircraftSelected->Specials)
	{
		if (!IsValid(Data)) continue;
		USpecialButtonWidget* Card = CreateWidget<USpecialButtonWidget>(GetWorld(), SpecialButtonClass);
		if (!IsValid(Card)) continue;
		Card->Setup(Data);
		Card->OnSpecialPicked.AddDynamic(SpecialUI, &USpecialSelectionComponent::SetSpecial);
		SpecialScrollBox->AddChild(Card);
	}
	if (!IsValid(Advancebtn)) return;
	Advancebtn->OnClicked.AddDynamic(this, &USpecialSelectionWidget::OnAdvancePicked);
}

void USpecialSelectionWidget::OnAdvancePicked() 
{
	OnAdvance.Broadcast();
}

void USpecialSelectionWidget::HandleSpecialSelected(TSubclassOf<UBaseSpecial> Special)
{
	OnWidgetSelected.Broadcast(Special);
}