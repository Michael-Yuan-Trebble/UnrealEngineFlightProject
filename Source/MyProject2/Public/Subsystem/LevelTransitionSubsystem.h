// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Structs and Data/MissionData.h"
#include "Enums/TakeoffType.h"
#include "LevelTransitionSubsystem.generated.h"

UCLASS(Blueprintable)
class MYPROJECT2_API ULevelTransitionSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	void LoadMission(const TSoftObjectPtr<UWorld> Level);
	void LoadMainMenu();
	void RestartMission();
	void TransitionScreen();

	void LoadIntermission(const ETakeoffType& TakeoffType);

private:
};
