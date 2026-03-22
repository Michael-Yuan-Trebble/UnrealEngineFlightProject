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

	BuyButton = CreateWidget<UBuyButton>(GetWorld(), BuyButtonClass);
	UAircraftStats* Loaded = AircraftData->AircraftStat.LoadSynchronous();

	if (!IsValid(BuyButton) || !IsValid(AircraftData) || !IsValid(Loaded)) return;

	BuyButton->Setup(Loaded->AircraftName, AircraftData->price);
	BuyButton->OnBuyPressed.AddDynamic(BuyUI, &UBuySelectionComponent::BuyAircraft);

	if (CurrentCurrency < AircraftData->price)
	{
		BuyButton->TurnOffBuy();
	}
	if (IsValid(SizeBox)) SizeBox->AddChild(BuyButton);
} 

void UBuyPopupWidget::NativeDestruct() {
	if (IsValid(BuyButton)) BuyButton->OnBuyPressed.RemoveAll(this);
	Super::NativeDestruct();
}