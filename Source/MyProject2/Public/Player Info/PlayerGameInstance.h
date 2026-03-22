// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Structs and Data/Aircraft Data/AircraftStats.h"
#include "Structs and Data/LoadoutInfo/AircraftLoadoutData.h"
#include "Structs and Data/MissionInfo/MissionData.h"
#include "Structs and Data/Aircraft Data/AircraftDatabase.h"
#include "Enums/LevelType.h"
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

	void SetLoadout(const FAircraftLoadoutData& InLoadout) { FullLoadout = InLoadout; };

	const FAircraftLoadoutData& GetLoadout() { return FullLoadout; };

	void FadeIn(ELevelType InType);

	UFUNCTION()
	void HandleFadeFinished();

	void FadeOut();

	void GoToLevel();

	void ShowTransition();

	void HideTransition();

	void CreateTransition();

	UFUNCTION()
	void HandlePostLoad(UWorld* LoadedWorld);

	UAircraftDatabase* GetDatabase() const { return AircraftDatabase; };

private:

	UPROPERTY(EditAnywhere)
	TObjectPtr<UAircraftDatabase> AircraftDatabase = nullptr;

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

	void SetMaps();

	virtual void Init() override;

	virtual void Shutdown() override;

	FAircraftLoadoutData FullLoadout{};

	ELevelType LevelType = ELevelType::Main;

	void CreateFade();
};
