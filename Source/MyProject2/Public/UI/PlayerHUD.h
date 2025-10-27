// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "LockBoxWidget.h"
#include "AircraftPlayerController.h"
#include "PlayerHUD.generated.h"

class ABaseAircraft;

UCLASS()
class MYPROJECT2_API APlayerHUD : public AHUD
{
	GENERATED_BODY()
public:

	APlayerHUD();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds);

	UPROPERTY()
	TSubclassOf<ULockBoxWidget> LockBoxWidgetClass;

	UPROPERTY()
	AAircraftPlayerController* PC;

	UPROPERTY()
	TArray<ABaseAircraft*> Targets;

	UPROPERTY()
	ULockBoxWidget* SelectedAircraftWidget = nullptr;

	APawn* Controlled;

	UPROPERTY()
	TMap<ABaseAircraft*, ULockBoxWidget*> ActiveWidgets;

	void UpdateLocked(bool Locked);

	void UpdateSelected(ABaseAircraft* In);

	UFUNCTION()
	void OnWeaponCountChanged(int32 Current, int32 Max);

	void Init();

	UPROPERTY(BlueprintReadWrite)
	int CurrentNum = 0;

	UPROPERTY(BlueprintReadWrite)
	int MaxNum = 0;

private:

	void UpdateTargetWidgets();

};
