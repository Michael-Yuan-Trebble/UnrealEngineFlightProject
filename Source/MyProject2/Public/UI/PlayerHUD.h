// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "LockBoxWidget.h"
#include "AircraftPlayerController.h"
#include "PlayerHUD.generated.h"

UCLASS()
class MYPROJECT2_API APlayerHUD : public AHUD
{
	GENERATED_BODY()
public:

	APlayerHUD();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds);

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ULockBoxWidget> LockBoxWidgetClass;

	UPROPERTY(EditDefaultsOnly)
	AAircraftPlayerController* PC;

private:
	TMap<AActor*, UUserWidget*> ActiveWidgets;
	TMap<AActor*, FVector2D> TargetScreenPositions;
	void UpdateTargetWidgets();

};
