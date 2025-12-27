// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Structs and Data/AircraftAnimationInformation.h"
#include "AircraftVisualComponent.generated.h"

class ACountermeasureActor;

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYPROJECT2_API UAircraftVisualComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UAircraftVisualComponent();

	void SetThrust(float In) { InputThrust = In; };
	void SetPitch(float In) { InputPitch = In; };
	void SetYaw(float In) { InputYaw = In; };
	void SetRoll(float In) { InputRoll = In; };

	float GetRudder() const { return Rudder; };
	float GetSlat() const { return Slat; };
	float GetLFlap() const { return LFlap; };
	float GetRFlap() const { return RFlap; };
	float GetNozzle() const { return Nozzle; };
	float GetAirBrake() const { return AirBrake; };
	float GetElevator() const { return Elevator; };
	bool IsCountermeasures() const { return bUsingCountermeasures; };

	USkeletalMeshComponent* Mesh;

	void ActivateFlares();

	UPROPERTY(EditAnywhere)
	TSubclassOf<ACountermeasureActor> FlareClass;

	UPROPERTY(EditAnywhere)
	bool bUsingCountermeasures;

public:
	void SetMesh(USkeletalMeshComponent* In) { Mesh = In; };

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

	float LFlapRoll;
	float RFlapRoll;

	float FlapPitch;

	float ElevatorRoll;
	float ElevatorPitch;

	float Rudder;
	float Slat;
	float Flap;

	float LFlap;
	float RFlap;

	float Nozzle;
	float AirBrake;
	float Elevator;
};
