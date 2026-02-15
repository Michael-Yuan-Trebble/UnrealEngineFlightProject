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

	void SetThrust(const float In) { InputThrust = In; };
	void SetPitch(const float In) { InputPitch = In; };
	void SetYaw(const float In) { InputYaw = In; };
	void SetRoll(const float In) { InputRoll = In; };

	float GetRudder() const { return Rudder; };
	float GetSlat() const { return Slat; };
	float GetLFlap() const { return LFlap; };
	float GetRFlap() const { return RFlap; };
	float GetNozzle() const { return Nozzle; };
	float GetAirBrake() const { return AirBrake; };
	float GetElevator() const { return Elevator; };
	bool IsCountermeasures() const { return bUsingCountermeasures; };

	UPROPERTY()
	USkeletalMeshComponent* Mesh = nullptr;

	void ActivateFlares();

	UPROPERTY(EditAnywhere)
	TSubclassOf<ACountermeasureActor> FlareClass = nullptr;

	UPROPERTY(EditAnywhere)
	bool bUsingCountermeasures;

public:
	void SetMesh(USkeletalMeshComponent* In) { Mesh = In; };

protected:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void PitchCalculation(const float D);

	virtual void RollCalculation(const float D);

	virtual void YawCalculation(const float D);

	virtual void AirBrakeCalculation(const float D);

	virtual void ThrustCalculation(const float D);

	UPROPERTY(EditAnywhere)
	FAircraftAnimationInformation AircraftInfo;

	UPROPERTY(EditAnywhere)
	bool bCanards = false;

	UPROPERTY(EditAnywhere)
	bool bElevators = false;

	UPROPERTY(EditAnywhere)
	bool bNozzle = false;

	UPROPERTY(EditAnywhere)
	bool bSlats = false;

private:
	float InputThrust = 0.f;
	float InputPitch = 0.f;
	float InputYaw = 0.f;
	float InputRoll = 0.f;

	float LFlapRoll = 0.f;
	float RFlapRoll = 0.f;

	float FlapPitch = 0.f;

	float ElevatorRoll = 0.f;
	float ElevatorPitch = 0.f;

	float Rudder = 0.f;
	float Slat = 0.f;
	float Flap = 0.f;

	float LFlap = 0.f;
	float RFlap = 0.f;

	float Nozzle = 0.f;
	float AirBrake = 0.f;
	float Elevator = 0.f;

	static constexpr int InterpSpeed = 30;
};
