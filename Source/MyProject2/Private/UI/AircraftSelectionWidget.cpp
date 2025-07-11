// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Widget!"));
#include "UI/AircraftSelectionWidget.h"
#include "Components/ScrollBox.h"
#include "Structs and Data/AircraftDatabase.h"
#include "AircraftPlayerController.h"
#include "MenuManagerComponent.h"
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

void UAircraftSelectionWidget::GetAllAircraft() 
{
	if (!AircraftButtonClass || !AircraftScrollBox || AircraftDatabase->AllAircraft.Num() <= 0) return;

    AircraftScrollBox->ClearChildren();

    AAircraftSelectionGamemode* Gamemode = Cast<AAircraftSelectionGamemode>(UGameplayStatics::GetGameMode(this));
    if (!Gamemode) return;

    AAircraftPlayerController* PC = Cast<AAircraftPlayerController>(UGameplayStatics::GetPlayerController(Gamemode, 0));
    if (!PC) return; 

    MenuManager = PC->MenuManager;
    if (!MenuManager) return;

    for (UAircraftData* Data : AircraftDatabase->AllAircraft)
    {
        if (!Data) continue;

        UAircraftButtonWidget* Card = CreateWidget<UAircraftButtonWidget>(GetWorld(), AircraftButtonClass);
        if (!Card) continue;

        Card->Setup(Data, CurrentMoney, Owned);
        Card->OnAircraftSelected.AddDynamic(this, &UAircraftSelectionWidget::HandleAircraftSelected);

        if (Owned.Contains(Data->AircraftName)) 
        {
            Card->OnAircraftPicked.AddDynamic(MenuManager, &UMenuManagerComponent::SetAircraft);
        }
        else 
        {
            Card->OnBuyCreate.AddDynamic(MenuManager, &UMenuManagerComponent::BuyAircraft);
        }

        AircraftScrollBox->AddChild(Card);
    }
    
    Gamemode->SpawnInAircraft(AircraftDatabase->AllAircraft[0]->AircraftClass);
}

void UAircraftSelectionWidget::HandleAircraftSelected(TSubclassOf<APawn> Aircraft) 
{
    if (!Aircraft) return;
    OnAircraftSelected.Broadcast(Aircraft);
}