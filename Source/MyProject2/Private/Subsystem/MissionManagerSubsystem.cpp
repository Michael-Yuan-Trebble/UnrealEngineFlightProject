// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("SET LEVEL!"));
#include "Subsystem/MissionManagerSubsystem.h"

void UMissionManagerSubsystem::SetCurrentMission(const FMissionData & InMission) {
	print()
	CurrentMission = InMission;
}