// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/SelectionUI/AircraftSelect/AircraftSelectionWidget.h"
#include "Components/ScrollBox.h"
#include "Structs and Data/Aircraft Data/AircraftDatabase.h"
#include "Units/Components/Player/MenuManagerComponent.h"
#include "UI/SelectionUI/AircraftSelect/AircraftSelectionComponent.h"
#include "Player Info/SaveGameManager.h"
#include "Player Info/PlayerGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Gamemodes/AircraftSelectionGamemode.h"
#include "UI/SelectionUI/AircraftSelect/AircraftButtonWidget.h"
#include "Debug/DebugHelper.h"

UAircraftSelectionWidget::UAircraftSelectionWidget(const FObjectInitializer & ObjectInitializer) : Super(ObjectInitializer)
{
}

void UAircraftSelectionWidget::Setup(UMenuManagerComponent* InMenu, UAircraftSelectionComponent* InSelect)
{
    MenuManager = InMenu;
    AircraftUI = InSelect;
}

void UAircraftSelectionWidget::GetAllAircraft() 
{
    if (!IsValid(AircraftButtonClass) || !IsValid(AircraftScrollBox) || !IsValid(MenuManager) || !IsValid(GetWorld())) return;

    UPlayerGameInstance* GI = Cast<UPlayerGameInstance>(GetWorld()->GetGameInstance());

    if (!IsValid(GI)) return;

    TArray<FName> Owned = GI->SaveManager->GetAircraftOwned();
    AircraftScrollBox->ClearChildren();

    for (UAircraftData* Data : GI->GetDatabase()->AllAircraft)
    {
        if (!IsValid(Data) || !Data->AircraftStat) continue;

        UAircraftButtonWidget* Card = CreateWidget<UAircraftButtonWidget>(GetWorld(), AircraftButtonClass);
        if (!IsValid(Card)) continue;

        bool bOwned = Owned.Contains(Data->AircraftStat->AircraftName);

        Card->Setup(Data, bOwned);
        Card->OnAircraftSelected.AddDynamic(this, &UAircraftSelectionWidget::HandleAircraftSelected);

        // TODO: Make this into one function that then decides, or maybe bind delegates for this too
        if (bOwned)
            Card->OnAircraftPicked.AddDynamic(AircraftUI, &UAircraftSelectionComponent::SetAircraft);
        else 
            Card->OnBuyCreate.AddDynamic(MenuManager, &UMenuManagerComponent::SpawnBuy);

        AircraftScrollBox->AddChild(Card);
        ButtonArray.Add(Data->AircraftStat->AircraftName, Card);
    }

    if (auto* Gamemode = Cast<AAircraftSelectionGamemode>(UGameplayStatics::GetGameMode(this))) 
    {
        if (GI->GetDatabase()->AllAircraft.Num() > 0 && IsValid(GI->GetDatabase()->AllAircraft[0]))
        {
            Gamemode->SpawnInAircraft(GI->GetDatabase()->AllAircraft[0]->AircraftClass);
        }
    }
}

void UAircraftSelectionWidget::UpdateAircraft(const FName& AircraftChange) 
{
    if (ButtonArray.Contains(AircraftChange)) 
    {
        UAircraftButtonWidget* Card = *ButtonArray.Find(AircraftChange);
        if (!IsValid(Card)) return;
        Card->AdjustButtons();
        if (IsValid(MenuManager)) Card->OnBuyCreate.RemoveDynamic(MenuManager, &UMenuManagerComponent::SpawnBuy);
        if (IsValid(AircraftUI)) Card->OnAircraftPicked.AddDynamic(AircraftUI, &UAircraftSelectionComponent::SetAircraft);
    }
}

void UAircraftSelectionWidget::HandleAircraftSelected(UAircraftData* Aircraft)
{
    OnWidgetSelected.Broadcast(Aircraft);
}