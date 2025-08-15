// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Widget!"));
#include "UI/AircraftSelectionWidget.h"
#include "Components/ScrollBox.h"
#include "Structs and Data/AircraftDatabase.h"
#include "AircraftPlayerController.h"
#include "MenuManagerComponent.h"
#include "UI/AircraftSelectionComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Gamemodes/AircraftSelectionGamemode.h"
#include "UI/AircraftButtonWidget.h"


UAircraftSelectionWidget::UAircraftSelectionWidget(const FObjectInitializer & ObjectInitializer) : Super(ObjectInitializer)
{
    static ConstructorHelpers::FClassFinder<UUserWidget> AircraftButtonBPClass(TEXT("/Game/Widgets/BPAircraftButton"));
    if (AircraftButtonBPClass.Succeeded())
    {
        AircraftButtonClass = AircraftButtonBPClass.Class;
    }
}

void UAircraftSelectionWidget::Setup(UAircraftDatabase* Database, TArray<FName> InOwn, UMenuManagerComponent* InMenu, UAircraftSelectionComponent* InSelect)
{
    Owned = InOwn;
    AircraftDatabase = Database;
    MenuManager = InMenu;
    AircraftUI = InSelect;
}

void UAircraftSelectionWidget::GetAllAircraft() 
{
	if (!AircraftButtonClass || !AircraftScrollBox || AircraftDatabase->AllAircraft.Num() <= 0) return;

    AircraftScrollBox->ClearChildren();

    AAircraftSelectionGamemode* Gamemode = Cast<AAircraftSelectionGamemode>(UGameplayStatics::GetGameMode(this));
    if (!Gamemode) return;

    if (!MenuManager) return;

    for (UAircraftData* Data : AircraftDatabase->AllAircraft)
    {
        if (!Data) continue;

        UAircraftButtonWidget* Card = CreateWidget<UAircraftButtonWidget>(GetWorld(), AircraftButtonClass);
        if (!Card) continue;

        Card->Setup(Data, Owned);
        Card->OnAircraftSelected.AddDynamic(this, &UAircraftSelectionWidget::HandleAircraftSelected);

        if (Owned.Contains(Data->AircraftName)) 
        {
            Card->OnAircraftPicked.AddDynamic(AircraftUI, &UAircraftSelectionComponent::SetAircraft);
        }
        else 
        {
            Card->OnBuyCreate.AddDynamic(MenuManager, &UMenuManagerComponent::SpawnBuy);
        }

        AircraftScrollBox->AddChild(Card);
        ButtonArray.Add(Data->AircraftName, Card);
    }
    Gamemode->SpawnInAircraft(AircraftDatabase->AllAircraft[0]->AircraftClass);
}

void UAircraftSelectionWidget::UpdateAircraft(FName AircraftChange) 
{
    if (ButtonArray.Contains(AircraftChange)) 
    {
        UAircraftButtonWidget* Card = *ButtonArray.Find(AircraftChange);

        Card->AdjustButtons();
        Card->OnBuyCreate.RemoveDynamic(MenuManager, &UMenuManagerComponent::SpawnBuy);
        Card->OnAircraftPicked.AddDynamic(AircraftUI, &UAircraftSelectionComponent::SetAircraft);
    }
}

void UAircraftSelectionWidget::HandleAircraftSelected(UAircraftData* Aircraft)
{
    if (!Aircraft) return;
    OnWidgetSelected.Broadcast(Aircraft);
}