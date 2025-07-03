// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Gamemode!"));
#include "Gamemodes/AircraftSelectionGamemode.h"
#include "GameFramework/SpectatorPawn.h"
#include "UI/AircraftSelectionWidget.h"
#include "AircraftPlayerController.h"
#include "Structs and Data/ControlModeTypes.h"
#include "Kismet/GameplayStatics.h"

AAircraftSelectionGamemode::AAircraftSelectionGamemode() 
{
	static ConstructorHelpers::FClassFinder<UUserWidget> WidgetBPClass(TEXT("/Game/UI/BPAircraftSelectionWidget"));
	if (WidgetBPClass.Succeeded())
	{
		WidgetClass = WidgetBPClass.Class;
	}
}

void AAircraftSelectionGamemode::BeginPlay() {

	Super::BeginPlay();

	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (PC) {
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		ASpectatorPawn* Spectator = GetWorld()->SpawnActor<ASpectatorPawn>(ASpectatorPawn::StaticClass(), FVector::ZeroVector,FRotator::ZeroRotator,SpawnParams);
		if (Spectator) {
			PC->Possess(Spectator);
			if (AAircraftPlayerController* APC = Cast<AAircraftPlayerController>(PC)) {
				APC->SetControlMode(EControlMode::Menu);
			}
		}
	}

	AircraftSelectUI = CreateWidget<UAircraftSelectionWidget>(GetWorld(), WidgetClass);
	AircraftDatabase = NewObject<UAircraftDatabase>(GetGameInstance());

	if (AircraftDatabase) 
	{
		FString Path = "/Game/Aircraft/AircraftData";
		AircraftDatabase->LoadAllAircraftFromFolder(Path);
	}

	if (AircraftSelectUI && AircraftDatabase) 
	{
		AircraftSelectUI->AircraftDatabase = AircraftDatabase;
		AircraftSelectUI->GetAllAircraft();
		AircraftSelectUI->AddToViewport();
		PC = GetWorld()->GetFirstPlayerController();
		if (PC) 
		{
			FInputModeGameAndUI InputMode;
			InputMode.SetWidgetToFocus(AircraftSelectUI->TakeWidget()); 
			InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			InputMode.SetHideCursorDuringCapture(true);
			PC->SetInputMode(InputMode);
			PC->bShowMouseCursor = true;
		}
	}
}

void AAircraftSelectionGamemode::PickedAircraft(UAircraftData* Aircraft) {
	if (AircraftSelectUI && Aircraft) 
	{
		AircraftSelectUI->RemoveFromViewport();
		ChosenAircraft = Aircraft;
	}
}