// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gamemodes/FlightGamemode.h"
#include "TakeoffGamemode.generated.h"

UCLASS()
class MYPROJECT2_API ATakeoffGamemode : public AFlightGamemode
{
	GENERATED_BODY()
	
public:
	ATakeoffGamemode();

private:
	virtual void BeginPlay() override;

	virtual void Tick(float D) override;

	void CheckTakeoff(const float D);

	UPROPERTY(EditAnywhere)
	float Distance = 3000.f;

	UPROPERTY(EditAnywhere)
	float TakeoffTimer = 3.f;
};
