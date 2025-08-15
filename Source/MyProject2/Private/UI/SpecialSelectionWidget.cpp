// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SpecialSelectionWidget.h"
#include "Structs and Data/AircraftDatabase.h"
#include "AircraftPlayerController.h"
#include "MenuManagerComponent.h"
#include "UI/SpecialSelectionComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UI/SpecialButtonWidget.h"

USpecialSelectionWidget::USpecialSelectionWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<UUserWidget> SpecialButtonBPClass(TEXT("/Game/Widgets/BPSpecialButton"));
	if (SpecialButtonBPClass.Succeeded())
	{
		SpecialButtonClass = SpecialButtonBPClass.Class;
	}
}

void USpecialSelectionWidget::Setup(UAircraftData* InAircraft, UMenuManagerComponent* InMenu, USpecialSelectionComponent* InSelect) 
{
	AircraftSelected = InAircraft;
	MenuManager = InMenu;
	SpecialUI = InSelect;
}

void USpecialSelectionWidget::GetAllSpecials() 
{
	if (!SpecialButtonClass || !SpecialScrollBox) return;
	SpecialScrollBox->ClearChildren();

	if (!MenuManager) return;

	for (TSubclassOf<UBaseSpecial> Data : AircraftSelected->Specials)
	{
		if (!Data) continue;

		USpecialButtonWidget* Card = CreateWidget<USpecialButtonWidget>(GetWorld(), SpecialButtonClass);
		if (!Card) continue;
		Card->Setup(Data);
		Card->OnSpecialPicked.AddDynamic(SpecialUI, &USpecialSelectionComponent::SetSpecial);
		SpecialScrollBox->AddChild(Card);
	}
}

void USpecialSelectionWidget::HandleSpecialSelected(TSubclassOf<UBaseSpecial> Special)
{
	if (!Special) return;
	OnWidgetSelected.Broadcast(Special);
}