// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Widget!"));
#include "UI/AircraftSelectionWidget.h"
#include "Components/ScrollBox.h"
#include "Structs and Data/AircraftDatabase.h"
#include "Gamemodes/AircraftSelectionGamemode.h"
#include "Kismet/GameplayStatics.h"
#include "UI/AircraftButtonWidget.h"


UAircraftSelectionWidget::UAircraftSelectionWidget(const FObjectInitializer & ObjectInitializer) : Super(ObjectInitializer)
{
       static ConstructorHelpers::FClassFinder<UUserWidget> AircraftButtonBPClass(TEXT("/Game/Widgets/BPAircraftButton"));
       if (AircraftButtonBPClass.Succeeded())
       {
           AircraftButtonClass = AircraftButtonBPClass.Class;
       }
}

void UAircraftSelectionWidget::GetAllAircraft() {
	if (!AircraftButtonClass || !AircraftScrollBox) return;

    AircraftScrollBox->ClearChildren();

    for (UAircraftData* Data : AircraftDatabase->AllAircraft)
    {
        if (!Data) continue;

        UAircraftButtonWidget* Card = CreateWidget<UAircraftButtonWidget>(GetWorld(), AircraftButtonClass);
        if (Card)
        {
            Card->Setup(Data);

            Card->OnAircraftSelected.AddDynamic(this, &UAircraftSelectionWidget::HandleAircraftSelected);
            AAircraftSelectionGamemode* Gamemode = Cast<AAircraftSelectionGamemode>(UGameplayStatics::GetGameMode(this));
            if (Gamemode) {
                Card->OnAircraftPicked.AddDynamic(Gamemode, &AAircraftSelectionGamemode::PickedAircraft);
            }

            AircraftScrollBox->AddChild(Card);
        }
    }
    if (!CurrentPreview) {
        FActorSpawnParameters SpawnParams;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        FVector PreviewLocation = FVector::ZeroVector;
        FRotator PreviewRotation = FRotator::ZeroRotator;
        if (AircraftDatabase->AllAircraft.Num() > 0) 
        {
            CurrentPreview = GetWorld()->SpawnActor<APawn>(AircraftDatabase->AllAircraft[0]->AircraftClass, PreviewLocation, PreviewRotation, SpawnParams);
        }
    }
}

void UAircraftSelectionWidget::HandleAircraftSelected(TSubclassOf<APawn> Aircraft) {
    if (CurrentPreview && CurrentPreview->GetClass() == Aircraft) return;
    if (CurrentPreview && CurrentPreview->IsValidLowLevel()) 
    {
        CurrentPreview->Destroy();
        CurrentPreview = nullptr;
    }
    if (!Aircraft) return;

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    FVector PreviewLocation = FVector::ZeroVector;
    FRotator PreviewRotation = FRotator::ZeroRotator;

    CurrentPreview = GetWorld()->SpawnActor<APawn>(Aircraft, PreviewLocation, PreviewRotation, SpawnParams);
}