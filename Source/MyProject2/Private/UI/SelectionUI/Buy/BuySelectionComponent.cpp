// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SelectionUI/Buy/BuySelectionComponent.h"
#include "UI/SelectionUI/Buy/BuyPopupWidget.h"
#include "Player Info/AircraftPlayerController.h"
#include "Player Info/PlayerGameInstance.h"
#include "Player Info/SaveGameManager.h"
#include "UI/SelectionUI/AircraftSelect/AircraftSelectionWidget.h"
#include "UI/SelectionUI/AircraftSelect/AircraftSelectionComponent.h"
#include "Debug/DebugHelper.h"

UBuySelectionComponent::UBuySelectionComponent()
{
}

void UBuySelectionComponent::AddAircraft(UAircraftData* Data, UAircraftSelectionComponent* InUI) 
{
	Aircraft = Data;
	AircraftUI = InUI;
}

void UBuySelectionComponent::BuyPopupMenu() 
{
	if (!IsValid(GreyOutClass) || !IsValid(BuyPopupClass) || !IsValid(GetWorld())) return;

	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!IsValid(PC)) return;

	GreyOut = CreateWidget<UUserWidget>(PC, GreyOutClass);
	if (!IsValid(GreyOut)) return;
	GreyOut->AddToViewport(0);

	BuyWidget = CreateWidget<UBuyPopupWidget>(PC, BuyPopupClass);
	if (!IsValid(BuyWidget)) return;
	BuyWidget->SetComp(this);
	BuyWidget->Setup(Aircraft);
	BuyWidget->AddToViewport(1);

	FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputMode.SetHideCursorDuringCapture(true);
	if (IsValid(BuyWidget)) InputMode.SetWidgetToFocus(BuyWidget->TakeWidget());
	PC->SetInputMode(InputMode);
	PC->bShowMouseCursor = true;
}

void UBuySelectionComponent::BuyAircraft(bool bBought)
{
	if (!IsValid(GetWorld())) return;
	AAircraftPlayerController* PC = Cast<AAircraftPlayerController>(GetWorld()->GetFirstPlayerController());
	UPlayerGameInstance* GI = Cast<UPlayerGameInstance>(GetWorld()->GetGameInstance());
	if (!IsValid(PC) || !IsValid(GI)) return;

	if (bBought) {
		if (!IsValid(GI) || !IsValid(GI->SaveManager) || !IsValid(Aircraft) || Aircraft->AircraftStat.IsNull()) return;
		UAircraftStats* LoadedData = Aircraft->AircraftStat.LoadSynchronous();
		const FName& AircraftName = LoadedData->AircraftName;

		GI->SaveManager->AddAircraftOwned(AircraftName);
		GI->SaveManager->AddMoney(-Aircraft->price);
		if (!IsValid(AircraftUI)) return;
		AircraftUI->UpdateAircraft(AircraftName);
	}

	FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputMode.SetHideCursorDuringCapture(true);
	if (IsValid(AircraftUI) && IsValid(AircraftUI->GetWidget())) InputMode.SetWidgetToFocus(AircraftUI->GetWidget()->TakeWidget());
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
			BuyWidget->RemoveFromParent();
		BuyWidget = nullptr;
	}

	if (IsValid(GreyOut))
	{
		if (GreyOut->IsInViewport())
			GreyOut->RemoveFromParent();
		GreyOut = nullptr;
	}
}