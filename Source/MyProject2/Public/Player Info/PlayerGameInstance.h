// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Weapons/BaseWeapon.h"
#include "Specials/BaseSpecial.h"
#include "Structs and Data/Aircraft Data/AircraftStats.h"
#include "Structs and Data/AircraftLoadoutData.h"
#include "Structs and Data/MissionData.h"
#include "PlayerGameInstance.generated.h"

class UMainMenuManager;
class ABaseAircraft;
class UFadeWidget;

UCLASS()
class MYPROJECT2_API UPlayerGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:

	UPROPERTY()
	class USaveGameManager* SaveManager = nullptr;

	void SetLevel(const FMissionData& InLevel) { Level = InLevel; };

	const FMissionData& GetLevel() const { return Level; };

	void SetLoadout(const FAircraftLoadoutData& InLoadout) { FullLoadout = InLoadout; };

	const FAircraftLoadoutData& GetLoadout() { return FullLoadout; };

	void FadeIn();

	void FadeOut();

	bool DoesFadeExist() const;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UFadeWidget> FadeWidgetClass;

	UPROPERTY()
	UFadeWidget* FadeWidget;

private:

	FMissionData Level;

	void Init() override;

	FAircraftLoadoutData FullLoadout;

	void CreateFade();
};
