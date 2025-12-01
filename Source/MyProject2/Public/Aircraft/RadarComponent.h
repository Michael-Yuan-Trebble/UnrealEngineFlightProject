// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Structs and Data/FDetectedAircraftInfo.h"
#include "RadarComponent.generated.h"

class ABaseUnit;
class APlayerHUD;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRadarScanEvent, const TArray<FDetectedAircraftInfo>&, Targets);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYPROJECT2_API URadarComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	URadarComponent();

	FRadarScanEvent RadarScanEvent;

	void ScanTargets();

	void Setup(ABaseUnit* InControl);

	void CycleTarget();

	void CycleToNextTarget();

	void SetTarget(AActor* NewTarget);

	UFUNCTION()
	void HandleSelectedDestroyed();

	TArray<FDetectedAircraftInfo> Enemies;

	ABaseUnit* Controlled;

	UPROPERTY()
	ABaseUnit* Selected;

	APlayerHUD* HUD;

	FTimerHandle RadarScanTimer;
	
	ABaseUnit* LastSelected = nullptr;

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
