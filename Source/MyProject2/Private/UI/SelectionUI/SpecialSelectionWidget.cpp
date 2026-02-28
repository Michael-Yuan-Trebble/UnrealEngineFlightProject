// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SelectionUI/SpecialSelectionWidget.h"
#include "Structs and Data/Aircraft Data/AircraftDatabase.h"
#include "Player Info/AircraftPlayerController.h"
#include "Units/Aircraft/MenuManagerComponent.h"
#include "UI/SelectionUI/SpecialSelectionComponent.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/ScrollBox.h"
#include "Kismet/GameplayStatics.h"
#include "UI/SelectionUI/SpecialButtonWidget.h"

USpecialSelectionWidget::USpecialSelectionWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void USpecialSelectionWidget::Setup(UAircraftData* InAircraft, UMenuManagerComponent* InMenu, USpecialSelectionComponent* InSelect) 
{
	AircraftSelected = InAircraft;
	MenuManager = InMenu;
	SpecialUI = InSelect;
}

void USpecialSelectionWidget::GetAllSpecials() 
{
	if (!IsValid(SpecialButtonClass) || !IsValid(SpecialScrollBox)) return;
	SpecialScrollBox->ClearChildren();

	if (!IsValid(MenuManager)) return;

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
	if (!IsValid(Special)) return;
	OnWidgetSelected.Broadcast(Special);
}