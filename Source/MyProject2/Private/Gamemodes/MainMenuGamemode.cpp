// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Main Menu Gamemode!"));
#include "Gamemodes/MainMenuGamemode.h"
#include "Kismet/GameplayStatics.h"
#include "Player Info/AircraftPlayerController.h"
#include "UI/MainMenuUI/MainMenuWidget.h"
#include "UI/MainMenuUI/FreeFlightWidget.h"
#include "Gamemodes/MainMenuManager.h"

AMainMenuGamemode::AMainMenuGamemode() 
{

}

void AMainMenuGamemode::BeginPlay() 
{
	Super::BeginPlay();

	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (!PC) return;

	APC = Cast<AAircraftPlayerController>(PC);
	PS = Cast<ACurrentPlayerState>(PC->PlayerState);

	if (!APC || !PS) return;
	APC->SetControlMode(EControlMode::Menu);

	UMainMenuManager* MenuManager = GetWorld()->GetGameInstance()->GetSubsystem<UMainMenuManager>();
	if (!MenuManager) return;

	if (!MainMenuClass || !FreeFlightClass) return;

	MenuManager->MainMenuClass = MainMenuClass;
	MenuManager->FreeFlightClass = FreeFlightClass;
	MenuManager->ShowMainMenu();

}