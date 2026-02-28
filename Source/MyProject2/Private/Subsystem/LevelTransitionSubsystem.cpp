// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("LEVEL NOT FOUND!"));
#include "Subsystem/LevelTransitionSubsystem.h"
#include "Subsystem/MissionManagerSubsystem.h"
#include "Engine/AssetManager.h"
#include "Kismet/GameplayStatics.h"

void ULevelTransitionSubsystem::LoadMainMenu() {

}

void ULevelTransitionSubsystem::LoadMission(const TSoftObjectPtr<UWorld> Level) 
{
	UWorld* World = GetWorld();
	if (!IsValid(World)) return;
	FName LevelName;
	if (Level.IsNull()) 
	{
		if (UMissionManagerSubsystem* Sub = GetGameInstance()->GetSubsystem<UMissionManagerSubsystem>()) {
			if (Sub->GetDefaultMap().IsValid()) {
				LevelName = FName(*Level.GetAssetName());
			}
		}
	}
	else {
		LevelName = FName(*Level.GetAssetName());
	}
	
	UGameplayStatics::OpenLevel(World, LevelName);
}

void ULevelTransitionSubsystem::RestartMission() {
}

void ULevelTransitionSubsystem::TransitionScreen() {
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!IsValid(PC)) return;
	PC->SetInputMode(FInputModeUIOnly());
	PC->SetShowMouseCursor(true);
}

void ULevelTransitionSubsystem::LoadIntermission(const ETakeoffType& TakeoffType) 
{
	UMissionManagerSubsystem* Sub = GetGameInstance()->GetSubsystem<UMissionManagerSubsystem>();
	if (!IsValid(Sub)) return;
	switch (TakeoffType) {
		case ETakeoffType::Grounded:
			LoadMission(Sub->GetGroundTakeoffMap());
			break;
		case ETakeoffType::Naval:
			LoadMission(Sub->GetNavalCarrierMap());
			break;
		default:
			LoadMission(Sub->GetCurrentMission().Level);
	}
}