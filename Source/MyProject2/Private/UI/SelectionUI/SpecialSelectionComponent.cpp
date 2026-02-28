// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/SelectionUI/SpecialSelectionComponent.h"
#include "UI/SelectionUI/SpecialSelectionWidget.h"
#include "Player Info/AircraftPlayerController.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Player Info/PlayerGameInstance.h"
#include "Units/Aircraft/MenuManagerComponent.h"

USpecialSelectionComponent::USpecialSelectionComponent()
{
}

void USpecialSelectionComponent::Setup(AAircraftPlayerController* InPlayer, UMenuManagerComponent* InMenu)
{
	PC = InPlayer;
	MenuManager = InMenu;
	GameInstance = Cast<UPlayerGameInstance>(GetWorld()->GetGameInstance());
}

void USpecialSelectionComponent::SetAir(UAircraftData* InAir)
{
	SelectedAircraft = InAir;
}

void USpecialSelectionComponent::SpecialSelectionMenu() 
{
	if (!IsValid(PC)) return;

	SpecialSelectUI = CreateWidget<USpecialSelectionWidget>(PC, SelectionWidget);

	if (!IsValid(SpecialSelectUI)) return;

	SpecialSelectUI->Setup(SelectedAircraft, MenuManager, this);
	SpecialSelectUI->GetAllSpecials();

	FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputMode.SetHideCursorDuringCapture(true);

	SpecialSelectUI->AddToViewport();
	InputMode.SetWidgetToFocus(SpecialSelectUI->TakeWidget());
	PC->SetInputMode(InputMode);
	PC->bShowMouseCursor = true;
	MenuManager->CurrentWidget = SpecialSelectUI;
	SpecialSelectUI->OnWidgetSelected.AddDynamic(this, &USpecialSelectionComponent::SetSpecial);
	SpecialSelectUI->OnAdvance.AddDynamic(this, &USpecialSelectionComponent::AdvanceLevel);
}

void USpecialSelectionComponent::SetSpecial(TSubclassOf<UBaseSpecial> Special)
{
	if (IsValid(Special)) MenuManager->SelectedSpecial = Special;
}

void USpecialSelectionComponent::AdvanceLevel() 
{
	MenuManager->AdvanceToLevel();
}

void USpecialSelectionComponent::CloseAll() 
{
	if (IsValid(SpecialSelectUI))
	{
		SpecialSelectUI->OnWidgetSelected.RemoveAll(this);
		SpecialSelectUI->OnAdvance.RemoveAll(this);
		if (SpecialSelectUI->IsInViewport()) 
		{
			SpecialSelectUI->RemoveFromParent();
		}
		SpecialSelectUI = nullptr;
	}
}