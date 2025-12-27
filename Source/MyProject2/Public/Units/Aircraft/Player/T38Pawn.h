// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Units/Aircraft/BaseAircraft.h"
#include "Units/Aircraft/Player/PlayerAircraft.h"
#include "T38Pawn.generated.h"

UCLASS()
class MYPROJECT2_API AT38Pawn : public APlayerAircraft
{
	GENERATED_BODY()

public:
	AT38Pawn();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Rudder;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LeftTurningFlap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RightTurningFlap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LeftLandingFlap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RightLandingFlap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LeftElevator;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RightElevator;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LeftExhaust;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RightExhaust;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LeftAirBrake;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RightAirBrake;

	virtual void PossessedBy(AController* NewController);

protected:

	virtual void Tick(float DeltaTime) override;
private:
	void PitchCalculation(float DeltaSeconds);
	void RollCalculation(float DeltaSeconds);
	void YawCalculation(float DeltaSeconds);
	void AirBrakeCalculation(float DeltaSeconds);
	
	
};
