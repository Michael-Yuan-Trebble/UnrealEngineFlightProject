// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gamemodes/FlightGamemode.h"
#include "NavalTakeoffGamemode.generated.h"

class ACarrier;

UCLASS()
class MYPROJECT2_API ANavalTakeoffGamemode : public AFlightGamemode
{
	GENERATED_BODY()
	
public:

	ANavalTakeoffGamemode();

private:

	virtual void BeginPlay() override;

	virtual void Tick(float D) override;

	void CheckTakeoff(const float D);

	UPROPERTY(EditAnywhere)
	float Distance = 50000.f;

	UPROPERTY(EditAnywhere)
	float TakeoffTimer = 3.f;

	UPROPERTY(EditAnywhere)
	float CatapultTimer = 1.f;

	float CurrentCatapultTime = CatapultTimer;

	UPROPERTY(EditAnywhere)
	float LaunchTime = 1.f;

	UPROPERTY(EditAnywhere)
	float AddedSpeed = 5.f;

	bool bStatic = true;

	UPROPERTY(EditAnywhere)
	TSubclassOf<ACarrier> CarrierClass;

	UPROPERTY()
	ACarrier* Carrier;

	UPROPERTY(EditAnywhere)
	FVector SpawnVector = FVector(0,0,150);

	virtual void HandlePlayerSpawnPoint() override;

	void CatapultTakeoff(const float D);

	void TransitionLevel();
};
