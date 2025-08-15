// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/AircraftSelectionComponent.h"
#include "UI/AircraftSelectionWidget.h"
#include "SaveGameManager.h"
#include "Gamemodes/PlayerGameInstance.h"
#include "MenuManagerComponent.h"
#include "Gamemodes/AircraftSelectionGamemode.h"
#include "AircraftPlayerController.h"

UAircraftSelectionComponent::UAircraftSelectionComponent()
{ 
	static ConstructorHelpers::FClassFinder<UUserWidget> WidgetBPClass(TEXT("/Game/UI/BPAircraftSelectionWidget"));
	if (WidgetBPClass.Succeeded())
	{
		SelectionWidget = WidgetBPClass.Class;
	}
}

void UAircraftSelectionComponent::Setup(AAircraftPlayerController* InPlayer, AAircraftSelectionGamemode* InGM, ACurrentPlayerState* InPS, UMenuManagerComponent* InMenu) 
{
	PC = InPlayer;
	GM = InGM;
	PS = InPS;
	MenuManager = InMenu;
	GameInstance = Cast<UPlayerGameInstance>(GetWorld()->GetGameInstance());
	
	AircraftDatabase = NewObject<UAircraftDatabase>(PC->GetGameInstance());
	if (!AircraftDatabase) return;

	FString Path = "/Game/Aircraft/AircraftData";
	AircraftDatabase->LoadAllAircraftFromFolder(Path);
}

void UAircraftSelectionComponent::AircraftSelectionMenu() 
{
	if (PC == nullptr) return;

	AircraftSelectUI = CreateWidget<UAircraftSelectionWidget>(PC, SelectionWidget);

	if (!AircraftSelectUI || !AircraftDatabase) return;

	AircraftSelectUI->Setup(AircraftDatabase, GameInstance->SaveManager->GetAircraftOwned(), MenuManager, this);
	AircraftSelectUI->GetAllAircraft();

	FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputMode.SetHideCursorDuringCapture(true);

	AircraftSelectUI->AddToViewport();
	InputMode.SetWidgetToFocus(AircraftSelectUI->TakeWidget());
	PC->SetInputMode(InputMode);
	PC->bShowMouseCursor = true;
	MenuManager->CurrentWidget = AircraftSelectUI;
	AircraftSelectUI->OnWidgetSelected.AddDynamic(this, &UAircraftSelectionComponent::HandleAircraftPicked);
}

void UAircraftSelectionComponent::HandleAircraftPicked(UAircraftData* Aircraft) 
{
	if (!Aircraft) return;
	GM->SpawnInAircraft(Aircraft->AircraftClass);
	MenuManager->TempAircraft = Aircraft;
}

void UAircraftSelectionComponent::SetAircraft(UAircraftData* Aircraft)
{
	if (!Aircraft) return;
	MenuManager->SelectedAircraft = Aircraft;
	PS->SetCurrentAircraft(Aircraft);
	MenuManager->ChooseWeaponUI();
}

void UAircraftSelectionComponent::UpdateAircraft(FName Name) 
{
	AircraftSelectUI->UpdateAircraft(Name);
}

void UAircraftSelectionComponent::CloseAll()
{
	if (IsValid(AircraftSelectUI))
	{
		if (AircraftSelectUI->IsInViewport())
		{
			AircraftSelectUI->RemoveFromParent();
		}
		AircraftSelectUI = nullptr;
	}
}