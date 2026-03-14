// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SelectionUI/Buy/BuyPopupWidget.h"
#include "UI/SelectionUI/Buy/BuyButton.h"
#include "Components/SizeBox.h"
#include "UI/SelectionUI/Buy/BuySelectionComponent.h"

UBuyPopupWidget::UBuyPopupWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) 
{
}

void UBuyPopupWidget::Setup(UAircraftData* AircraftData)
{
	if (!IsValid(BuyButtonClass)) return;

	UBuyButton* Button = CreateWidget<UBuyButton>(GetWorld(), BuyButtonClass);
	UAircraftStats* Loaded = AircraftData->AircraftStat.LoadSynchronous();

	if (!IsValid(Button) || !IsValid(AircraftData) || !IsValid(Loaded)) return;

	Button->Setup(Loaded->AircraftName, AircraftData->price);
	Button->OnBuyPressed.AddDynamic(BuyUI, &UBuySelectionComponent::BuyAircraft);

	if (CurrentCurrency < AircraftData->price)
	{
		Button->TurnOffBuy();
	}
	if (IsValid(SizeBox)) SizeBox->AddChild(Button);
} 