// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/SelectionUI/AircraftSelect/AircraftSelectionComponent.h"
#include "UI/SelectionUI/AircraftSelect/AircraftSelectionWidget.h"
#include "Units/Components/Player/MenuManagerComponent.h"
#include "Gamemodes/AircraftSelectionGamemode.h"
#include "Kismet/GameplayStatics.h"
#include "Debug/DebugHelper.h"

UAircraftSelectionComponent::UAircraftSelectionComponent()
{ 
}

void UAircraftSelectionComponent::Setup(UMenuManagerComponent* InMenu)
{
	MenuManager = InMenu;
}

void UAircraftSelectionComponent::AircraftSelectionMenu() 
{
	if (!IsValid(SelectionWidget) || !IsValid(GetWorld()) || IsValid(AircraftSelectUI)) return;

	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC) return;

	AircraftSelectUI = CreateWidget<UAircraftSelectionWidget>(PC, SelectionWidget);
	if (!IsValid(AircraftSelectUI)) return;

	AircraftSelectUI->Setup(MenuManager, this);
	AircraftSelectUI->GetAllAircraft();

	FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputMode.SetHideCursorDuringCapture(true);

	AircraftSelectUI->AddToViewport();
	InputMode.SetWidgetToFocus(AircraftSelectUI->TakeWidget());

	PC->SetInputMode(InputMode);
	PC->bShowMouseCursor = true;
	AircraftSelectUI->OnWidgetSelected.AddDynamic(this, &UAircraftSelectionComponent::HandleAircraftPicked);
}

void UAircraftSelectionComponent::HandleAircraftPicked(UAircraftData* Aircraft) 
{
	if (!IsValid(Aircraft) || !IsValid(MenuManager)) return;
	if (AAircraftSelectionGamemode* GM = Cast<AAircraftSelectionGamemode>(UGameplayStatics::GetGameMode(this))) {
		GM->SpawnInAircraft(Aircraft->AircraftClass);
	}
	MenuManager->SetTempAircraft(Aircraft);
}

void UAircraftSelectionComponent::SetAircraft(UAircraftData* Aircraft)
{
	if (!IsValid(MenuManager)) return;
	MenuManager->SetAircraft(Aircraft);
	MenuManager->ChooseWeaponUI();
}

void UAircraftSelectionComponent::UpdateAircraft(const FName& Name) 
{
	if (IsValid(AircraftSelectUI)) AircraftSelectUI->UpdateAircraft(Name);
}

void UAircraftSelectionComponent::CloseAll()
{
	if (!IsValid(AircraftSelectUI)) return;
	AircraftSelectUI->OnWidgetSelected.RemoveAll(this);
	if (AircraftSelectUI->IsInViewport())
	{
		AircraftSelectUI->RemoveFromParent();
	}
	AircraftSelectUI = nullptr;
}

UUserWidget* UAircraftSelectionComponent::GetWidget() const { return AircraftSelectUI; }