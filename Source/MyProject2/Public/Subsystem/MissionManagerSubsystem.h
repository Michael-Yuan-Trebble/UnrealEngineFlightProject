// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Enums/TakeoffType.h"
#include "Structs and Data/MissionData.h"
#include "MissionManagerSubsystem.generated.h"

UCLASS()
class MYPROJECT2_API UMissionManagerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:

	FMissionData GetCurrentMission() const { return CurrentMission; };

	void SetCurrentMission(const FMissionData& InMission);

	void SetAircraftSelectMap(TSoftObjectPtr<UWorld> InWorld) { AircraftSelectMap = InWorld; };

	void SetNavalCarrierMap(TSoftObjectPtr<UWorld> InWorld) { NavalCarrierMap = InWorld; };

	void SetGroundTakeoffMap(TSoftObjectPtr<UWorld> InWorld) { GroundTakeoffMap = InWorld; };

	void SetDefaultMap(TSoftObjectPtr<UWorld> InWorld) { DefaultMap = InWorld; };

	TSoftObjectPtr<UWorld> GetAircraftSelectMap() const { return AircraftSelectMap; };

	TSoftObjectPtr<UWorld> GetNavalCarrierMap() const { return NavalCarrierMap; };

	TSoftObjectPtr<UWorld> GetGroundTakeoffMap() const { return GroundTakeoffMap; };

	TSoftObjectPtr<UWorld> GetDefaultMap() const { return DefaultMap; };

private:
	ETakeoffType CurrentTakeoffType = ETakeoffType::Airborne;

	FMissionData CurrentMission{};

	UPROPERTY()
	TSoftObjectPtr<UWorld> AircraftSelectMap = nullptr;

	UPROPERTY()
	TSoftObjectPtr<UWorld> NavalCarrierMap = nullptr;

	UPROPERTY()
	TSoftObjectPtr<UWorld> GroundTakeoffMap = nullptr;

	UPROPERTY()
	TSoftObjectPtr<UWorld> DefaultMap = nullptr;
};
