// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Structs and Data/AircraftAnimationInformation.h"
#include "AircraftVisualComponent.generated.h"


UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYPROJECT2_API UAircraftVisualComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UAircraftVisualComponent();

	UPROPERTY(BlueprintReadWrite)
	float Rudder;

	UPROPERTY(BlueprintReadWrite)
	float Slat;

	UPROPERTY(BlueprintReadWrite)
	float Flap;

	UPROPERTY(BlueprintReadWrite)
	float Nozzle;

	UPROPERTY(BlueprintReadWrite)
	float AirBrake;

	UPROPERTY(BlueprintReadWrite)
	float Elevator;

	void SetThrust(float In) { InputThrust = In; };
	void SetPitch(float In) { InputPitch = In; };
	void SetYaw(float In) { InputYaw = In; };
	void SetRoll(float In) { InputRoll = In; };

protected:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void PitchCalculation(float D);

	virtual void RollCalculation(float D);

	virtual void YawCalculation(float D);

	virtual void AirBrakeCalculation(float D);

	virtual void ThrustCalculation(float D);

	UPROPERTY(EditAnywhere)
	FAircraftAnimationInformation AircraftInfo;

	UPROPERTY(EditAnywhere)
	bool bCanards;

	UPROPERTY(EditAnywhere)
	bool bElevators;

	UPROPERTY(EditAnywhere)
	bool bNozzle;

	UPROPERTY(EditAnywhere)
	bool bSlats;

private:
	float InputThrust;
	float InputPitch;
	float InputYaw;
	float InputRoll;

	float FlapRoll;
	float FlapPitch;

	float ElevatorRoll;
	float ElevatorPitch;
};
