// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SpecialSelectionComponent.h"
#include "UI/SpecialSelectionWidget.h"
#include "AircraftPlayerController.h"
#include "Gamemodes/PlayerGameInstance.h"
#include "MenuManagerComponent.h"

USpecialSelectionComponent::USpecialSelectionComponent()
{
	static ConstructorHelpers::FClassFinder<UUserWidget> WidgetBPClass(TEXT("/Game/UI/BPSpecialSelectionWidget"));
	if (WidgetBPClass.Succeeded())
	{
		SelectionWidget = WidgetBPClass.Class;
	}
}

void USpecialSelectionComponent::Setup(AAircraftPlayerController* InPlayer, ACurrentPlayerState* InPS, UMenuManagerComponent* InMenu)
{
	PC = InPlayer;
	PS = InPS;
	MenuManager = InMenu;
	GameInstance = Cast<UPlayerGameInstance>(GetWorld()->GetGameInstance());
}

void USpecialSelectionComponent::SetAir(UAircraftData* InAir)
{
	SelectedAircraft = InAir;
}

void USpecialSelectionComponent::SpecialSelectionMenu() 
{
	if (PC == nullptr) return;

	SpecialSelectUI = CreateWidget<USpecialSelectionWidget>(PC, SelectionWidget);

	if (!SpecialSelectUI) return;

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
}

void USpecialSelectionComponent::SetSpecial(TSubclassOf<UBaseSpecial> Special)
{
	if (!Special) return;
	MenuManager->SelectedSpecial = Special;
	PS->SetSpecial(Special);
}

void USpecialSelectionComponent::CloseAll() 
{
	if (IsValid(SpecialSelectUI))
	{
		if (SpecialSelectUI->IsInViewport()) 
		{
			SpecialSelectUI->RemoveFromParent();
		}
		SpecialSelectUI = nullptr;
	}
}