// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Main Menu Gamemode!"));
#include "Gamemodes/MainMenuGamemode.h"
#include "Kismet/GameplayStatics.h"
#include "Player Info/AircraftPlayerController.h"
#include "UI/MainMenuUI/MainMenuWidget.h"
#include "UI/MainMenuUI/FreeFlightWidget.h"
#include "Gamemodes/MainMenuManager.h"
#include "Subsystem/LevelTransitionSubsystem.h"
#include "Subsystem/MissionManagerSubsystem.h"
#include "Player Info/PlayerGameInstance.h"

AMainMenuGamemode::AMainMenuGamemode() 
{

}

void AMainMenuGamemode::BeginPlay() 
{
	Super::BeginPlay();

	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (!PC) return;

	APC = Cast<AAircraftPlayerController>(PC);

	if (!APC) return;
	APC->SetControlMode(EControlMode::Menu);

	UMainMenuManager* MenuManager = GetWorld()->GetGameInstance()->GetSubsystem<UMainMenuManager>();
	if (!MenuManager) return;

	if (!IsValid(MainMenuClass) || !IsValid(FreeFlightClass)) return;

	MenuManager->SetMainMenuClass(MainMenuClass);
	MenuManager->SetFreeFlightClass(FreeFlightClass);
	MenuManager->ShowMainMenu();

}

void AMainMenuGamemode::LevelSelected(FMissionData Level) {

	UWorld* World = GetWorld();
	if (!IsValid(World)) return;

	UPlayerGameInstance* GI = World->GetGameInstance<UPlayerGameInstance>();
	if (!IsValid(GI)) return;
	ULevelTransitionSubsystem* LevelSystem = GI->GetSubsystem<ULevelTransitionSubsystem>();
	UMissionManagerSubsystem* MissionSystem = GI->GetSubsystem<UMissionManagerSubsystem>();
	if (!IsValid(LevelSystem) || !IsValid(MissionSystem)) return;

	GI->SetLevel(Level);

	LevelSystem->LoadMission(MissionSystem->GetAircraftSelectMap());
}