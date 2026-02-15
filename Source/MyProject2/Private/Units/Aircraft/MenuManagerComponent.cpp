// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("MenuManager!"));
#include "Units/Aircraft/MenuManagerComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Player Info/PlayerGameInstance.h"
#include "UI/SelectionUI/AircraftSelectionComponent.h"
#include "Gamemodes/AircraftSelectionGamemode.h"
#include "UI/SelectionUI/AircraftSelectionWidget.h"
#include "UI/SelectionUI/WeaponSelectionWidget.h"
#include "UI/SelectionUI/WeaponSelectionComponent.h"
#include "UI/SelectionUI/SpecialSelectionComponent.h"
#include "UI/SelectionUI/SpecialSelectionWidget.h"
#include "Player Info/AircraftPlayerController.h"
#include "UI/SelectionUI/BuySelectionComponent.h"
#include "UI/SelectionUI/BuyPopupWidget.h"

FInputModeGameAndUI InputMode;

UMenuManagerComponent::UMenuManagerComponent()
{
}

void UMenuManagerComponent::InitializePC(AAircraftPlayerController* InPC)
{
	PC = InPC;
	GM = Cast<AAircraftSelectionGamemode>(UGameplayStatics::GetGameMode(this));
	GameInstance = Cast<UPlayerGameInstance>(GetWorld()->GetGameInstance());

	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputMode.SetHideCursorDuringCapture(true);

	AircraftSelectionUI = NewObject<UAircraftSelectionComponent>(this);
	AircraftSelectionUI->Setup(PC, GM, this, AircraftSelectClass);

	WeaponSelectionUI = NewObject<UWeaponSelectionComponent>(this);
	WeaponSelectionUI->Setup(PC, GM, this);

	BuySelectionUI = NewObject<UBuySelectionComponent>(this);
	BuySelectionUI->Setup(PC, GameInstance);

	SpecialSelectionUI = NewObject<USpecialSelectionComponent>(this);
	SpecialSelectionUI->Setup(PC, this);
}

void UMenuManagerComponent::SetupClasses(TSubclassOf<UUserWidget> InAircraftClass,
	TSubclassOf<UUserWidget> InWeaponClass,
	TSubclassOf<UUserWidget> InBuyClass,
	TSubclassOf<UUserWidget> InSpecialClass) 
{
	AircraftSelectClass = InAircraftClass;
	WeaponSelectClass = InWeaponClass;
	BuySelectionClass = InBuyClass;
	SpecialSelectionClass = InSpecialClass;

	AircraftSelectionUI->SelectionWidget = InAircraftClass;
	WeaponSelectionUI->SelectionWidget = InWeaponClass;
	BuySelectionUI->BuyPopupClass = InBuyClass;
	SpecialSelectionUI->SelectionWidget = InSpecialClass;
}

void UMenuManagerComponent::GoBack(const EMenuState Current)
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

void UMenuManagerComponent::GetWidgetClassForState(const EMenuState State)
{
	if (State != EMenuState::BuyPopup && IsValid(CurrentWidget) && CurrentWidget->IsInViewport()) 
	{
		CurrentWidget->RemoveFromParent();
		CurrentWidget = nullptr;
	}

	switch (State) 
	{
	case EMenuState::AircraftSelect:
		AircraftSelectionUI->AircraftSelectionMenu();
		CurrentWidget = AircraftSelectionUI->AircraftSelectUI;
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
			CurrentWidget = SpecialSelectionUI->SpecialSelectUI;
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
	if (IsValid(PC)) PC->ManageMenuSetting(EMenuState::AircraftSelect);
}

void UMenuManagerComponent::ChooseWeaponUI() 
{
	if (IsValid(PC)) PC->ManageMenuSetting(EMenuState::WeaponSelect);
}

void UMenuManagerComponent::ChooseSpecialUI() 
{
	if (IsValid(PC)) PC->ManageMenuSetting(EMenuState::SpecialSelect);
}

void UMenuManagerComponent::SpawnBuy(UAircraftData* AircraftData, const int Cost)
{
	TempAircraft = AircraftData;
	PC->MenuHistory.Push(EMenuState::BuyPopup);
	GetWidgetClassForState(EMenuState::BuyPopup);
}

void UMenuManagerComponent::EndSelection() 
{
	if (IsValid(GM)) GM->EndSelection(PC);
}

void UMenuManagerComponent::AdvanceToLevel() 
{
	if (IsValid(GM)) GM->TryAdvanceToNextStage();
}

void UMenuManagerComponent::CloseAll() 
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearAllTimersForObject(this);
	}

	if (IsValid(AircraftSelectionUI)) AircraftSelectionUI->CloseAll();
	if (IsValid(WeaponSelectionUI)) WeaponSelectionUI->CloseAll();
	if (IsValid(BuySelectionUI)) BuySelectionUI->CloseAll();
	if (IsValid(SpecialSelectionUI)) SpecialSelectionUI->CloseAll();

	AircraftSelectionUI = nullptr;
	WeaponSelectionUI = nullptr;
	BuySelectionUI = nullptr;
	SpecialSelectionUI = nullptr;

	if (APlayerController* TempPC = Cast<APlayerController>(GetOwner()))
	{
		TempPC->SetInputMode(FInputModeGameOnly());
		TempPC->bShowMouseCursor = false;
	}
}