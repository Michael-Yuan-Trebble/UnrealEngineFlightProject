// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Aircraft/Player/PlayerAircraft.h"
#include "Mirage2000Pawn.generated.h"

UCLASS()
class MYPROJECT2_API AMirage2000Pawn : public APlayerAircraft
{
	GENERATED_BODY()
	
public:
	AMirage2000Pawn();

	UPROPERTY(BlueprintReadWrite)
	float Rudder;

	UPROPERTY(BlueprintReadWrite)
	float Slat;
	
	UPROPERTY(BlueprintReadWrite)
	float Flap;

	UPROPERTY(BlueprintReadWrite)
	float TopAirBrake;

	UPROPERTY(BlueprintReadWrite)
	float BottomAirbrake;

	UPROPERTY(BlueprintReadWrite)
	float Nozzle = 0.7;

	float TotalFlapPitch;
	float TotalFlapRoll;

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
