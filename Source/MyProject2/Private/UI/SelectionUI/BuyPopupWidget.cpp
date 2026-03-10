// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SelectionUI/BuyPopupWidget.h"
#include "UI/SelectionUI/BuyButton.h"
#include "Components/SizeBox.h"
#include "UI/SelectionUI/BuySelectionComponent.h"

UBuyPopupWidget::UBuyPopupWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) 
{
}

void UBuyPopupWidget::Setup(UAircraftData* AircraftData)
{
	Aircraft = AircraftData;

	if (!IsValid(BuyButtonClass)) return;

	UBuyButton* Button = CreateWidget<UBuyButton>(GetWorld(), BuyButtonClass);

	UAircraftStats* Loaded = Aircraft->AircraftStat.LoadSynchronous();

	if (!IsValid(Button) || !IsValid(Aircraft) || !IsValid(Loaded)) return;

	Button->Setup(Loaded->AircraftName, Aircraft->price);
	Button->OnBuyPicked.AddDynamic(BuyUI, &UBuySelectionComponent::BuyAircraft);
	Button->OnCancelPicked.AddDynamic(BuyUI, &UBuySelectionComponent::CancelBuy);

	if (CurrentCurrency < Aircraft->price) 
	{
		Button->TurnOffBuy();
	}
	if (!IsValid(SizeBox)) return;
	SizeBox->AddChild(Button);
} 