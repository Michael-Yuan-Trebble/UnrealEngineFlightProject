// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SelectionUI/BuyPopupWidget.h"
#include "UI/SelectionUI/BuyButton.h"
#include "Gamemodes/AircraftSelectionGamemode.h"
#include "Kismet/GameplayStatics.h"
#include "AircraftPlayerController.h"
#include "Aircraft/MenuManagerComponent.h"
#include "UI/SelectionUI/BuySelectionComponent.h"

UBuyPopupWidget::UBuyPopupWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) 
{
	static ConstructorHelpers::FClassFinder<UUserWidget> BuyButtonBPClass(TEXT("/Game/Widgets/BPBuyButton"));
	if (BuyButtonBPClass.Succeeded()) 
	{
		BuyButtonClass = BuyButtonBPClass.Class;
	}
}

void UBuyPopupWidget::Setup(UAircraftData* AircraftData)
{
	Aircraft = AircraftData;
	
	AAircraftSelectionGamemode* Gamemode = Cast<AAircraftSelectionGamemode>(UGameplayStatics::GetGameMode(this));
	if (!Gamemode) return;

	UBuyButton* Button = CreateWidget<UBuyButton>(GetWorld(), BuyButtonClass);
	if (!Button) return;

	Button->Setup(Aircraft->AircraftName, Aircraft->price);
	Button->OnBuyPicked.AddDynamic(BuyUI, &UBuySelectionComponent::BuyAircraft);
	Button->OnCancelPicked.AddDynamic(BuyUI, &UBuySelectionComponent::CancelBuy);

	if (CurrentCurrency < Aircraft->price) 
	{
		Button->TurnOffBuy();
	}
	SizeBox->AddChild(Button);
} 