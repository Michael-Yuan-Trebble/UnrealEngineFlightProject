// Fill out your copyright notice in the Description page of Project Settings.

#include "Subsystem/LevelTransitionSubsystem.h"
#include "Subsystem/MissionManagerSubsystem.h"
#include "Engine/AssetManager.h"
#include "Kismet/GameplayStatics.h"

void ULevelTransitionSubsystem::LoadMainMenu() {

}

void ULevelTransitionSubsystem::LoadAircraftSelect() {
	UWorld* World = GetWorld();
	if (!IsValid(World)) return;

	if (auto* Sub = GetGameInstance()->GetSubsystem<UMissionManagerSubsystem>()) {
		const TSoftObjectPtr<UWorld>& Map = Sub->GetAircraftSelectMap();
		if (Map.IsNull()) return;
		UGameplayStatics::OpenLevelBySoftObjectPtr(World, Map);
	}
}

void ULevelTransitionSubsystem::LoadMission(const TSoftObjectPtr<UWorld> Level) {
	UWorld* World = GetWorld();
	if (!IsValid(World)) return;
	
	// Default Map Fallback
	if (Level.IsNull()) {
		if (auto* Sub = GetGameInstance()->GetSubsystem<UMissionManagerSubsystem>()) {
			const TSoftObjectPtr<UWorld>& Map = Sub->GetDefaultMap();
			if (Map.IsNull()) return;
			UGameplayStatics::OpenLevelBySoftObjectPtr(World, Map);
		}
	}
	else {
		UGameplayStatics::OpenLevelBySoftObjectPtr(World, Level);
	}
}

void ULevelTransitionSubsystem::RestartMission() {
}

void ULevelTransitionSubsystem::TransitionScreen() {
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!IsValid(PC)) return;
	PC->SetInputMode(FInputModeUIOnly());
	PC->SetShowMouseCursor(true);
}

void ULevelTransitionSubsystem::LoadIntermission(const ETakeoffType& TakeoffType) {
	auto* Sub = GetGameInstance()->GetSubsystem<UMissionManagerSubsystem>();
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