// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "LockBoxWidget.h"
#include "UI/HitNotificationWidget.h"
#include "MinimapWidget.h"
#include "Structs and Data/FDetectedAircraftInfo.h"
#include "PitchLadder.h"
#include "PlayerHUD.generated.h"

class APlayerAircraft;
class ABaseUnit;
class ABaseMissile;
class AAircraftPlayerController;

UCLASS()
class MYPROJECT2_API APlayerHUD : public AHUD
{
	GENERATED_BODY()
public:

	APlayerHUD();

	void UpdateLocked(const float LockPercent);

	void UpdateSelected();

	void TogglePitchLadder(bool Toggle);

	void UpdateTargetHit(bool bDestroyed);

	UFUNCTION()
	void OnWeaponChanged(FName WeaponName, int32 Current, int32 Max);

	void Init(AAircraftPlayerController* InPC);

	void SetTarget(TWeakObjectPtr<ABaseUnit> InTarget);

	void HandleMissileLaunchedAtSelf(ABaseMissile* Missile);
	
	void HandleMissileLockedAtSelf();

	UPROPERTY(BlueprintReadWrite)
	int CurrentNum = 0;

	UPROPERTY(BlueprintReadWrite)
	int MaxNum = 0;

	UPROPERTY(BlueprintReadWrite)
	FName CurrentName = NAME_None;

	bool isPitchLadderVisible = false;

	void HandleWeaponMiss();

	void OnUnitDestroyed(ABaseUnit* Death);

	UFUNCTION()
	void HandleRadarScan(const TArray<FDetectedAircraftInfo>& InEnemies);

private:

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditAnywhere)
	TSubclassOf<ULockBoxWidget> LockBoxWidgetClass = nullptr;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UMinimapWidget> MiniMapClass = nullptr;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> AimReticleClass = nullptr;

	UPROPERTY()
	TObjectPtr<UUserWidget> AimReticleWidget = nullptr;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> AOAReticleClass = nullptr;

	UPROPERTY()
	TObjectPtr<UUserWidget> AOAReticleWidget = nullptr;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UPitchLadder> PitchLadderClass = nullptr;

	UPROPERTY()
	TObjectPtr<UPitchLadder> PitchLadderWidget = nullptr;

	UPROPERTY()
	TObjectPtr<UMinimapWidget> MiniMap = nullptr;

	UPROPERTY()
	TObjectPtr<AAircraftPlayerController> PC = nullptr;

	UPROPERTY()
	TArray<TWeakObjectPtr<ABaseUnit>> Targets;

	UPROPERTY()
	TObjectPtr<ULockBoxWidget> SelectedAircraftWidget = nullptr;

	UPROPERTY()
	TWeakObjectPtr<APlayerAircraft> Controlled = nullptr;

	UPROPERTY()
	TWeakObjectPtr<ABaseUnit> LastActor = nullptr;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UHitNotificationWidget> HitNotiClass = nullptr;

	UPROPERTY()
	UHitNotificationWidget* HitNotiWidget = nullptr;

	UPROPERTY()
	TMap<TWeakObjectPtr<ABaseUnit>, TObjectPtr<ULockBoxWidget>> ActiveWidgets;

	UPROPERTY()
	TWeakObjectPtr<ABaseUnit> Target = nullptr;

	void UpdateTargetWidgets();

	void PitchLadderUpdate();

};
