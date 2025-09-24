// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Structs and Data/FDetectedAircraftInfo.h"
#include "RadarComponent.generated.h"

class ABaseAircraft;
class APlayerHUD;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYPROJECT2_API URadarComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	URadarComponent();

	void ScanTargets();

	void Setup(ABaseAircraft* InControl);

	void CycleTarget();

	void CycleToNextTarget();

	void SetTarget(AActor* NewTarget);

	TArray<FDetectedAircraftInfo> Detected;

	ABaseAircraft* Controlled;

	AActor* Selected;

	APlayerHUD* HUD;

	FTimerHandle RadarScanTimer;

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
