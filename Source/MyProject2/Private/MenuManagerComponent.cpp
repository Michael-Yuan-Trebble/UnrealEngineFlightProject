// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("MenuManager!"));
#include "MenuManagerComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Gamemodes/PlayerGameInstance.h"
#include "UI/AircraftSelectionComponent.h"
#include "Gamemodes/AircraftSelectionGamemode.h"
#include "UI/AircraftSelectionWidget.h"
#include "UI/WeaponSelectionWidget.h"
#include "UI/WeaponSelectionComponent.h"
#include "UI/SpecialSelectionComponent.h"
#include "AircraftPlayerController.h"
#include "UI/BuySelectionComponent.h"

FInputModeGameAndUI InputMode;

UMenuManagerComponent::UMenuManagerComponent()
{
}

void UMenuManagerComponent::InitializePC(AAircraftPlayerController* InPC, ACurrentPlayerState* InPS)
{
	PC = InPC;
	PS = InPS;
	GM = Cast<AAircraftSelectionGamemode>(UGameplayStatics::GetGameMode(this));
	GameInstance = Cast<UPlayerGameInstance>(GetWorld()->GetGameInstance());

	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputMode.SetHideCursorDuringCapture(true);

	AircraftSelectionUI = NewObject<UAircraftSelectionComponent>(this);
	AircraftSelectionUI->Setup(PC, GM, PS, this);

	WeaponSelectionUI = NewObject<UWeaponSelectionComponent>(this);
	WeaponSelectionUI->Setup(PC, GM, PS, this);

	BuySelectionUI = NewObject<UBuySelectionComponent>(this);
	BuySelectionUI->Setup(PC, GameInstance);

	SpecialSelectionUI = NewObject<USpecialSelectionComponent>(this);
	SpecialSelectionUI->Setup(PC, PS, this);
}

void UMenuManagerComponent::GoBack(EMenuState Current)
{
	switch (Current) 
	{
	case EMenuState::AircraftSelect:
		break;
	case EMenuState::WeaponSelect:
		break;
	case EMenuState::SpecialSelect:
		break;
	case EMenuState::BuyPopup:
		break;
	default:
		break;
	}

}

void UMenuManagerComponent::GetWidgetClassForState(EMenuState State)
{
	if (State != EMenuState::BuyPopup && IsValid(CurrentWidget) && CurrentWidget->IsInViewport()) 
	{
		CurrentWidget->RemoveFromParent();
		CurrentWidget = nullptr;
	}

	switch (State) 
	{
	case EMenuState::AircraftSelect:
		if (GM) 
		{
			AircraftSelectionUI->AircraftSelectionMenu();
			CurrentWidget = AircraftSelectionUI->AircraftSelectUI;
		}
		break;
	case EMenuState::WeaponSelect:
		if (SelectedAircraft) 
		{
			WeaponSelectionUI->AddAircraft(SelectedAircraft);
			WeaponSelectionUI->WeaponSelectionMenu();
			CurrentWidget = WeaponSelectionUI->WeaponSelectUI;
		}
		break;
	case EMenuState::SpecialSelect:
		if (SelectedAircraft) {
			SpecialSelectionUI->SetAir(SelectedAircraft);
			SpecialSelectionUI->SpecialSelectionMenu();
		}
		break;
	case EMenuState::BuyPopup:
		BuySelectionUI->AddAircraft(TempAircraft, AircraftSelectionUI);
		BuySelectionUI->BuyPopupMenu();
		break;
	default:
		break;
	}
}

void UMenuManagerComponent::ChooseAircraftUI() 
{
	PC->ManageMenuSetting(EMenuState::AircraftSelect);
}

void UMenuManagerComponent::ChooseWeaponUI() 
{
	PC->ManageMenuSetting(EMenuState::WeaponSelect);
}

void UMenuManagerComponent::ChooseSpecialUI() 
{
	PC->ManageMenuSetting(EMenuState::SpecialSelect);
}

void UMenuManagerComponent::SpawnBuy(UAircraftData* AircraftData, int Cost)
{
	TempAircraft = AircraftData;
	PC->MenuHistory.Push(EMenuState::BuyPopup);
	GetWidgetClassForState(EMenuState::BuyPopup);
}

void UMenuManagerComponent::EndSelection() 
{
	GM->EndSelection(PC);
}