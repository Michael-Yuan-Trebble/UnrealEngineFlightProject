// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Structs and Data/Aircraft Data/AircraftStats.h"
#include "Structs and Data/AircraftLoadoutData.h"
#include "Structs and Data/MissionData.h"
#include "PlayerGameInstance.generated.h"

class UMainMenuManager;
class ABaseAircraft;
class UFadeWidget;
class UTransitionWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FFadeFinishedSignature);

UCLASS()
class MYPROJECT2_API UPlayerGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:

	FFadeFinishedSignature OnFadeFinished;

	UPROPERTY()
	class USaveGameManager* SaveManager = nullptr;

	void SetLevel(const FMissionData& InLevel);

	const FMissionData& GetLevel() const { return Level; };

	void SetLoadout(const FAircraftLoadoutData& InLoadout) { FullLoadout = InLoadout; };

	const FAircraftLoadoutData& GetLoadout() { return FullLoadout; };

	void FadeIn();

	UFUNCTION()
	void HandleFadeFinished();

	void FadeOut();

	void GoToLevel();

	bool DoesFadeExist() const;

	void ShowTransition();

	void HideTransition();

	bool DoesTransitionExist() const;

	void CreateTransition();

	UFUNCTION()
	void HandlePostLoad(UWorld* LoadedWorld);

private:

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UFadeWidget> FadeWidgetClass = nullptr;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf< UTransitionWidget> TransitionClass = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Maps")
	TSoftObjectPtr<UWorld> AircraftSelectMap = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Maps")
	TSoftObjectPtr<UWorld> NavalCarrierMap = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Maps")
	TSoftObjectPtr<UWorld> GroundTakeoffMap = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Maps")
	TSoftObjectPtr<UWorld> DefaultMap = nullptr;

	UPROPERTY()
	TObjectPtr<UFadeWidget> FadeWidget = nullptr;

	UPROPERTY()
	TObjectPtr<UTransitionWidget> TransitionWidget = nullptr;

	void SetAircraftSelectMap();

	void SetNavalCarrierMap();

	void SetGroundTakeoffMap();

	void SetDefaultMap();

	FMissionData Level{};

	void Init() override;

	FAircraftLoadoutData FullLoadout{};

	void CreateFade();
};
