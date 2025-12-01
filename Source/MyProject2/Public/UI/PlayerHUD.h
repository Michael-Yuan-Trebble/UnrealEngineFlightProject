// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "LockBoxWidget.h"
#include "UI/HitNotificationWidget.h"
#include "MinimapWidget.h"
#include "Structs and Data/FDetectedAircraftInfo.h"
#include "PitchLadder.h"
#include "AircraftPlayerController.h"
#include "PlayerHUD.generated.h"

class APlayerAircraft;
class ABaseUnit;

UCLASS()
class MYPROJECT2_API APlayerHUD : public AHUD
{
	GENERATED_BODY()
public:

	APlayerHUD();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds);

	UPROPERTY(EditAnywhere)
	TSubclassOf<ULockBoxWidget> LockBoxWidgetClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UMinimapWidget> MiniMapClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> AimReticleClass;

	UUserWidget* AimReticleWidget;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> AOAReticleClass;

	UUserWidget* AOAReticleWidget;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UPitchLadder> PitchLadderClass;

	UPROPERTY()
	UPitchLadder* PitchLadderWidget;

	UMinimapWidget* MiniMap;

	UPROPERTY()
	AAircraftPlayerController* PC;

	UPROPERTY()
	TArray<ABaseUnit*> Targets;

	UPROPERTY()
	ULockBoxWidget* SelectedAircraftWidget = nullptr;

	APlayerAircraft* Controlled;

	TWeakObjectPtr<ABaseUnit> LastActor;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UHitNotificationWidget> HitNotiClass;

	UHitNotificationWidget* HitNotiWidget;

	UPROPERTY()
	TMap<ABaseUnit*, ULockBoxWidget*> ActiveWidgets;

	ABaseUnit* Target;

	void UpdateLocked(float LockPercent);

	void UpdateSelected();

	void TogglePitchLadder(bool Toggle);

	void UpdateTargetHit(bool bDestroyed);

	UFUNCTION()
	void OnWeaponChanged(FName WeaponName, int32 Current, int32 Max);

	void Init(AAircraftPlayerController* InPC);

	void SetTarget(TWeakObjectPtr<ABaseUnit> InTarget);

	UPROPERTY(BlueprintReadWrite)
	int CurrentNum = 0;

	UPROPERTY(BlueprintReadWrite)
	int MaxNum = 0;

	UPROPERTY(BlueprintReadWrite)
	FName CurrentName;

	bool isPitchLadderVisible;

	void HandleWeaponMiss();

	void OnUnitDestroyed(ABaseUnit* Death);

	UFUNCTION()
	void HandleRadarScan(const TArray<FDetectedAircraftInfo>& InEnemies);

private:

	void UpdateTargetWidgets();

	void PitchLadderUpdate();

};
