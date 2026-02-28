// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Widget!"));
#include "UI/SelectionUI/AircraftSelectionWidget.h"
#include "Components/ScrollBox.h"
#include "Structs and Data/Aircraft Data/AircraftDatabase.h"
#include "Player Info/AircraftPlayerController.h"
#include "Units/Aircraft/MenuManagerComponent.h"
#include "UI/SelectionUI/AircraftSelectionComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Gamemodes/AircraftSelectionGamemode.h"
#include "UI/SelectionUI/AircraftButtonWidget.h"


UAircraftSelectionWidget::UAircraftSelectionWidget(const FObjectInitializer & ObjectInitializer) : Super(ObjectInitializer)
{
}

void UAircraftSelectionWidget::Setup(
    UAircraftDatabase* Database, 
    TArray<FName> InOwn, 
    UMenuManagerComponent* InMenu, 
    UAircraftSelectionComponent* InSelect)
{
    Owned = InOwn;
    AircraftDatabase = Database;
    MenuManager = InMenu;
    AircraftUI = InSelect;
}

void UAircraftSelectionWidget::GetAllAircraft() 
{
    if (!IsValid(AircraftButtonClass) || !IsValid(AircraftScrollBox) || !IsValid(AircraftDatabase) || AircraftDatabase->AllAircraft.Num() <= 0) return;

    AircraftScrollBox->ClearChildren();

    AAircraftSelectionGamemode* Gamemode = Cast<AAircraftSelectionGamemode>(UGameplayStatics::GetGameMode(this));
    if (!IsValid(Gamemode) || !IsValid(MenuManager)) return;

    for (UAircraftData* Data : AircraftDatabase->AllAircraft)
    {
        if (!IsValid(Data)) continue;

        UAircraftButtonWidget* Card = CreateWidget<UAircraftButtonWidget>(GetWorld(), AircraftButtonClass);
        if (!Card) continue;

        Card->Setup(Data, Owned);
        Card->OnAircraftSelected.AddDynamic(this, &UAircraftSelectionWidget::HandleAircraftSelected);

        if (!Data->AircraftStat) continue;

        if (Owned.Contains(Data->AircraftStat->AircraftName))
        {
            Card->OnAircraftPicked.AddDynamic(AircraftUI, &UAircraftSelectionComponent::SetAircraft);
        }
        else 
        {
            Card->OnBuyCreate.AddDynamic(MenuManager, &UMenuManagerComponent::SpawnBuy);
        }

        AircraftScrollBox->AddChild(Card);
        ButtonArray.Add(Data->AircraftStat->AircraftName, Card);
    }
    if (AircraftDatabase->AllAircraft.Num() > 0 && IsValid(AircraftDatabase->AllAircraft[0])) 
    {
        Gamemode->SpawnInAircraft(AircraftDatabase->AllAircraft[0]->AircraftClass);
    }
}

void UAircraftSelectionWidget::UpdateAircraft(const FName& AircraftChange) 
{
    if (ButtonArray.Contains(AircraftChange)) 
    {
        UAircraftButtonWidget* Card = *ButtonArray.Find(AircraftChange);

        Card->AdjustButtons();
        if (IsValid(MenuManager)) Card->OnBuyCreate.RemoveDynamic(MenuManager, &UMenuManagerComponent::SpawnBuy);
        if (IsValid(AircraftUI)) Card->OnAircraftPicked.AddDynamic(AircraftUI, &UAircraftSelectionComponent::SetAircraft);
    }
}

void UAircraftSelectionWidget::HandleAircraftSelected(UAircraftData* Aircraft)
{
    if (IsValid(Aircraft)) OnWidgetSelected.Broadcast(Aircraft);
}