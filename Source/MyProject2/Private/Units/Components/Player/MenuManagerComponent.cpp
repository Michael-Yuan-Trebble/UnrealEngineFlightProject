// Fill out your copyright notice in the Description page of Project Settings.

#include "Units/Components/Player/MenuManagerComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Player Info/PlayerGameInstance.h"
#include "UI/SelectionUI/AircraftSelect/AircraftSelectionComponent.h"
#include "Gamemodes/AircraftSelectionGamemode.h"
#include "UI/SelectionUI/WeaponSelect/WeaponSelectionComponent.h"
#include "UI/SelectionUI/SpecialSelect/SpecialSelectionComponent.h"
#include "Player Info/AircraftPlayerController.h"
#include "UI/SelectionUI/Buy/BuySelectionComponent.h"
#include "Blueprint/UserWidget.h"
#include "Debug/DebugHelper.h"

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
	AircraftSelectionUI->Setup(this);

	WeaponSelectionUI = NewObject<UWeaponSelectionComponent>(this);
	WeaponSelectionUI->Setup(this);

	BuySelectionUI = NewObject<UBuySelectionComponent>(this);

	SpecialSelectionUI = NewObject<USpecialSelectionComponent>(this);
	SpecialSelectionUI->Setup(this);
}

void UMenuManagerComponent::SetupClasses(TSubclassOf<UUserWidget> InAircraftClass,
	TSubclassOf<UUserWidget> InWeaponClass,
	TSubclassOf<UUserWidget> InBuyClass,
	TSubclassOf<UUserWidget> InSpecialClass,
	TSubclassOf<UUserWidget> InGreyClass)
{
	AircraftSelectClass = InAircraftClass;
	WeaponSelectClass = InWeaponClass;
	BuySelectionClass = InBuyClass;
	SpecialSelectionClass = InSpecialClass;

	AircraftSelectionUI->SetWidget(InAircraftClass);
	WeaponSelectionUI->SetWeaponWidgetClass(InWeaponClass);
	BuySelectionUI->SetWidget(InBuyClass);
	BuySelectionUI->SetGrey(InGreyClass);
	SpecialSelectionUI->SetWidget(InSpecialClass);
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
		CurrentWidget = AircraftSelectionUI->GetWidget();
		break;
	case EMenuState::WeaponSelect:
		if (SelectedAircraft) 
		{
			WeaponSelectionUI->AddAircraft(SelectedAircraft);
			WeaponSelectionUI->WeaponSelectionMenu();
			CurrentWidget = WeaponSelectionUI->GetWeaponUI();
		}
		break;
	case EMenuState::SpecialSelect:
		if (SelectedAircraft) {
			SpecialSelectionUI->SetAir(SelectedAircraft);
			SpecialSelectionUI->SpecialSelectionMenu();
			CurrentWidget = SpecialSelectionUI->GetWidget();
		}
		break;
	case EMenuState::BuyPopup:
		BuySelectionUI->AddAircraft(TempAircraft, AircraftSelectionUI);
		BuySelectionUI->BuyPopupMenu();
		break;
	default:
		break;
	}
	if (IsValid(CurrentWidget) && !CurrentWidget->IsInViewport()) {
		CurrentWidget->AddToViewport();
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
	if (IsValid(PC)) PC->GetMenuHistory().Push(EMenuState::BuyPopup);
	GetWidgetClassForState(EMenuState::BuyPopup);
}

void UMenuManagerComponent::EndSelection() 
{
	if (IsValid(GM)) GM->EndSelection(PC);
}

void UMenuManagerComponent::AdvanceToLevel() 
{
	CloseAll();
	if (IsValid(GM)) GM->TryAdvanceToNextStage();
}

void UMenuManagerComponent::CloseAll() 
{
	if (UWorld* World = GetWorld())
		World->GetTimerManager().ClearAllTimersForObject(this);

	if (IsValid(AircraftSelectionUI)) AircraftSelectionUI->CloseAll();
	if (IsValid(WeaponSelectionUI)) WeaponSelectionUI->CloseAll();
	if (IsValid(BuySelectionUI)) BuySelectionUI->CloseAll();
	if (IsValid(SpecialSelectionUI)) SpecialSelectionUI->CloseAll();

	AircraftSelectionUI = nullptr;
	WeaponSelectionUI = nullptr;
	BuySelectionUI = nullptr;
	SpecialSelectionUI = nullptr;

	PC->SetInputMode(FInputModeGameOnly());
	PC->bShowMouseCursor = false;
}