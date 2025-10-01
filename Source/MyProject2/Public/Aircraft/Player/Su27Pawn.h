// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Aircraft/BaseAircraft.h"
#include "Aircraft/Player/PlayerAircraft.h"
#include "Su27Pawn.generated.h"

UCLASS()
class MYPROJECT2_API ASu27Pawn : public APlayerAircraft
{
	GENERATED_BODY()
	
public:
	ASu27Pawn();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator Rudder;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator Slat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator Flap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Nozzle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AirBrake;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float Elevator;

	virtual void PossessedBy(AController* NewController);

protected:
	virtual void Tick(float DeltaTime) override;

private:
	void PitchCalculation(float Delta);
	void RollCalculation(float Delta);
	void YawCalculation(float Delta);
	void AirBrakeCalculation(float Delta);
	void ThrustCalculation(float Delta);
};
