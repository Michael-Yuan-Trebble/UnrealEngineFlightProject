// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "LockBoxWidget.h"
#include "MinimapWidget.h"
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

	UMinimapWidget* MiniMap;

	UPROPERTY()
	AAircraftPlayerController* PC;

	UPROPERTY()
	TArray<ABaseUnit*> Targets;

	UPROPERTY()
	ULockBoxWidget* SelectedAircraftWidget = nullptr;

	APlayerAircraft* Controlled;

	UPROPERTY()
	TMap<ABaseUnit*, ULockBoxWidget*> ActiveWidgets;

	void UpdateLocked(bool Locked);

	void UpdateSelected(ABaseUnit* In);

	UFUNCTION()
	void OnWeaponChanged(FName WeaponName, int32 Current, int32 Max);

	void Init();

	UPROPERTY(BlueprintReadWrite)
	int CurrentNum = 0;

	UPROPERTY(BlueprintReadWrite)
	int MaxNum = 0;

	UPROPERTY(BlueprintReadWrite)
	FName CurrentName;

	UPROPERTY(BlueprintReadWrite)
	TArray<FName> EquippedWeaponNames;
private:

	void UpdateTargetWidgets();

};
