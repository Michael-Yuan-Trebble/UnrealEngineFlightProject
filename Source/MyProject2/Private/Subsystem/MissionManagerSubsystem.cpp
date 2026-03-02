// Fill out your copyright notice in the Description page of Project Settings.

#include "Subsystem/MissionManagerSubsystem.h"

void UMissionManagerSubsystem::SetCurrentMission(const FMissionData & InMission) {
	CurrentMission = InMission;
}