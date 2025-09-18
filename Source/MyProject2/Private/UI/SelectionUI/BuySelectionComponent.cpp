// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SelectionUI/BuySelectionComponent.h"
#include "UI/SelectionUI/BuyPopupWidget.h"
#include "AircraftPlayerController.h"
#include "Gamemodes/PlayerGameInstance.h"
#include "SaveGameManager.h"
#include "UI/SelectionUI/AircraftSelectionWidget.h"
#include "UI/SelectionUI/AircraftSelectionComponent.h"

UBuySelectionComponent::UBuySelectionComponent()
{
	static ConstructorHelpers::FClassFinder<UUserWidget> BuyBPClass(TEXT("/Game/UI/BPBuyPopupWidget"));
	if (BuyBPClass.Succeeded())
	{
		BuyPopupClass = BuyBPClass.Class;
	}
	static ConstructorHelpers::FClassFinder<UUserWidget> GreyOutBPClass(TEXT("/Game/UI/UIDim"));
	if (GreyOutBPClass.Succeeded())
	{
		GreyOutClass = GreyOutBPClass.Class;
	}
}

void UBuySelectionComponent::Setup(AAircraftPlayerController* InPlayer, UPlayerGameInstance* InGI) 
{
	PC = InPlayer;
	GI = InGI;
}

void UBuySelectionComponent::AddAircraft(UAircraftData* Data, UAircraftSelectionComponent* InUI) 
{
	Aircraft = Data;
	AircraftUI = InUI;
}

void UBuySelectionComponent::BuyPopupMenu() 
{
	GreyOut = CreateWidget<UUserWidget>(PC, GreyOutClass);
	if (!GreyOut) return;
	GreyOut->AddToViewport(0);

	BuyWidget = CreateWidget<UBuyPopupWidget>(PC, BuyPopupClass);
	if (!BuyWidget) return;
	BuyWidget->BuyUI = this;
	BuyWidget->Setup(Aircraft);
	BuyWidget->AddToViewport(1);

	FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputMode.SetHideCursorDuringCapture(true);
	InputMode.SetWidgetToFocus(BuyWidget->TakeWidget());
	PC->SetInputMode(InputMode);
	PC->bShowMouseCursor = true;
}

void UBuySelectionComponent::BuyAircraft(FName Name, int Cost)
{
	GI->SaveManager->AddAircraftOwned(Name);
	GI->SaveManager->AddMoney(-Cost);
	AircraftUI->UpdateAircraft(Name);

	FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputMode.SetHideCursorDuringCapture(true);
	InputMode.SetWidgetToFocus(AircraftUI->AircraftSelectUI->TakeWidget());
	PC->SetInputMode(InputMode);
	PC->bShowMouseCursor = true;
	CloseAll();
	PC->MenuHistory.Pop();
}

void UBuySelectionComponent::CancelBuy() 
{
	FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputMode.SetHideCursorDuringCapture(true);
	InputMode.SetWidgetToFocus(AircraftUI->AircraftSelectUI->TakeWidget());
	PC->SetInputMode(InputMode);
	PC->bShowMouseCursor = true;
	CloseAll();
	PC->MenuHistory.Pop();
}

void UBuySelectionComponent::CloseAll()
{

	if (IsValid(BuyWidget))
	{
		if (BuyWidget->IsInViewport())
		{
			BuyWidget->RemoveFromParent();
		}
		BuyWidget = nullptr;
	}

	if (IsValid(GreyOut))
	{
		if (GreyOut->IsInViewport())
		{
			GreyOut->RemoveFromParent();
		}
		GreyOut = nullptr;
	}
}