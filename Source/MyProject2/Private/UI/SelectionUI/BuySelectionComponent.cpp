// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SelectionUI/BuySelectionComponent.h"
#include "UI/SelectionUI/BuyPopupWidget.h"
#include "Player Info/AircraftPlayerController.h"
#include "Player Info/PlayerGameInstance.h"
#include "Player Info/SaveGameManager.h"
#include "UI/SelectionUI/AircraftSelectionWidget.h"
#include "UI/SelectionUI/AircraftSelectionComponent.h"

UBuySelectionComponent::UBuySelectionComponent()
{
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
	if (!IsValid(PC) || !IsValid(GreyOutClass)) return;
	GreyOut = CreateWidget<UUserWidget>(PC, GreyOutClass);
	if (!IsValid(GreyOut)) return;
	GreyOut->AddToViewport(0);

	if (!IsValid(PC) || !IsValid(BuyPopupClass)) return;
	BuyWidget = CreateWidget<UBuyPopupWidget>(PC, BuyPopupClass);
	if (!IsValid(BuyWidget)) return;
	BuyWidget->SetComp(this);
	BuyWidget->Setup(Aircraft);
	BuyWidget->AddToViewport(1);

	FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputMode.SetHideCursorDuringCapture(true);
	if (IsValid(BuyWidget)) InputMode.SetWidgetToFocus(BuyWidget->TakeWidget());
	if (!IsValid(PC)) return;
	PC->SetInputMode(InputMode);
	PC->bShowMouseCursor = true;
}

void UBuySelectionComponent::BuyAircraft(FName Name, int Cost)
{
	if (!IsValid(GI) || !IsValid(GI->SaveManager)) return;
	GI->SaveManager->AddAircraftOwned(Name);
	GI->SaveManager->AddMoney(-Cost);
	if (!IsValid(AircraftUI)) return;
	AircraftUI->UpdateAircraft(Name);

	FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputMode.SetHideCursorDuringCapture(true);
	InputMode.SetWidgetToFocus(AircraftUI->GetWidget()->TakeWidget());
	if (!IsValid(PC)) return;
	PC->SetInputMode(InputMode);
	PC->bShowMouseCursor = true;
	CloseAll();
	PC->GetMenuHistory().Pop();
}

void UBuySelectionComponent::CancelBuy() 
{
	FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputMode.SetHideCursorDuringCapture(true);
	if (IsValid(AircraftUI) && IsValid(AircraftUI->GetWidget())) InputMode.SetWidgetToFocus(AircraftUI->GetWidget()->TakeWidget());
	if (!IsValid(PC)) return;
	PC->SetInputMode(InputMode);
	PC->bShowMouseCursor = true;
	CloseAll();
	PC->GetMenuHistory().Pop();
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