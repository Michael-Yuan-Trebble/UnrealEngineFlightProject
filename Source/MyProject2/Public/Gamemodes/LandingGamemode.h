// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gamemodes/FlightGamemode.h"
#include "LandingGamemode.generated.h"

UCLASS()
class MYPROJECT2_API ALandingGamemode : public AFlightGamemode
{
	GENERATED_BODY()
	
public:
	ALandingGamemode();

	virtual void BeginPlay() override;

	virtual void Tick(float D) override;

private:
	void CheckLanding(float D);

	UPROPERTY(EditAnywhere)
	float LandingTimeCounter = 3;
};
