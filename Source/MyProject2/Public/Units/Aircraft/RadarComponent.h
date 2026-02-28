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

	const TArray<FDetectedAircraftInfo>& GetEnemies() const { return Enemies; };

	ABaseUnit* GetSelected() const { return Selected; };

private:
	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	TArray<FDetectedAircraftInfo> Enemies{};

	UPROPERTY()
	TObjectPtr<ABaseUnit> Controlled = nullptr;

	UPROPERTY()
	TObjectPtr<ABaseUnit> Selected = nullptr;

	UPROPERTY()
	TObjectPtr<APlayerHUD> HUD = nullptr;

	FTimerHandle RadarScanTimer;

	UPROPERTY()
	TObjectPtr<ABaseUnit> LastSelected = nullptr;

		
};
