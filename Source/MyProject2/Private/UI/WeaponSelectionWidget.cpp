// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("WeaponSelect!"));
#include "UI/WeaponSelectionWidget.h"
#include "Components/ScrollBox.h"
#include "Gamemodes/AircraftSelectionGamemode.h"
#include "Kismet/GameplayStatics.h"
#include "AircraftPlayerController.h"
#include "MenuManagerComponent.h"
#include "UI/WeaponButtonWidget.h"

UWeaponSelectionWidget::UWeaponSelectionWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    static ConstructorHelpers::FClassFinder<UUserWidget> WeaponButtonBPClass(TEXT("/Game/Widgets/BPWeaponSelectButton.BPWeaponSelectButton_C"));
    if (WeaponButtonBPClass.Succeeded())
    {
        WeaponButtonClass = WeaponButtonBPClass.Class;
    }
}

void UWeaponSelectionWidget::GetAllAircraft() 
{
    if (!WeaponButtonClass || !WeaponScrollBox) return;

    WeaponScrollBox->ClearChildren();

    AAircraftSelectionGamemode* Gamemode = Cast<AAircraftSelectionGamemode>(UGameplayStatics::GetGameMode(this));
    if (!Gamemode) return;

    AAircraftPlayerController* PC = Cast<AAircraftPlayerController>(UGameplayStatics::GetPlayerController(Gamemode, 0));
    if (!PC) return;

    MenuManager = PC->MenuManager;
    if (!MenuManager) return;

    CreateButtons(CurrentLoadout->AllowedMissiles);
    CreateButtons(CurrentLoadout->AllowedBombs);
    CreateButtons(CurrentLoadout->AllowedMisc);
}

void UWeaponSelectionWidget::CreateButtons(TArray<TSubclassOf<ABaseWeapon>> Array) 
{
    for (TSubclassOf<ABaseWeapon> SingleWeapon : Array)
    {
        if (!SingleWeapon) continue;

        UWeaponButtonWidget* Card = CreateWidget<UWeaponButtonWidget>(GetWorld(), WeaponButtonClass);

        if (!Card) return;

        Card->SetupWeapons(SingleWeapon);
        Card->OnWeaponSelected.AddDynamic(this, &UWeaponSelectionWidget::HandleWeaponSelected);
        Card->OnWeaponPicked.AddDynamic(MenuManager, &UMenuManagerComponent::AddWeapon);
        WeaponScrollBox->AddChild(Card);
    }
}

void UWeaponSelectionWidget::HandleWeaponSelected(TSubclassOf<ABaseWeapon> Weapon)
{
    if (Weapon) OnWeaponSelected.Broadcast(Weapon);
}