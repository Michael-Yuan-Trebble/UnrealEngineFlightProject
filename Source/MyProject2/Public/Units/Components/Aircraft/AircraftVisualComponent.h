// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Structs and Data/AircraftAnimationInformation.h"
#include "Structs and Data/AircraftAnimationValues.h"
#include "AircraftVisualComponent.generated.h"

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
	bool IsCountermeasures() const { return bUsingCountermeasures; };

	void ActivateFlares();

	void SetMesh(USkeletalMeshComponent* In) { Mesh = In; };

	const FAircraftAnimationValues& GetAircraftAnimationValues() const { return AircraftValues; };

private:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void PitchCalculation(const float D);

	virtual void RollCalculation(const float D);

	virtual void YawCalculation(const float D);

	virtual void AirBrakeCalculation(const float D);

	virtual void ThrustCalculation(const float D);

	UPROPERTY(EditAnywhere)
	FAircraftAnimationInformation AircraftInfo{};
	
	FAircraftAnimationValues AircraftValues{};

	UPROPERTY()
	TObjectPtr<USkeletalMeshComponent> Mesh = nullptr;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class ACountermeasureActor> FlareClass = nullptr;

	UPROPERTY(EditAnywhere)
	bool bUsingCountermeasures;

	UPROPERTY(EditAnywhere)
	bool bCanards = false;

	UPROPERTY(EditAnywhere)
	bool bElevators = false;

	UPROPERTY(EditAnywhere)
	bool bNozzle = false;

	UPROPERTY(EditAnywhere)
	bool bSlats = false;

	float InputThrust = 0.f;
	float InputPitch = 0.f;
	float InputYaw = 0.f;
	float InputRoll = 0.f;

	float LFlapRoll = 0.f;
	float RFlapRoll = 0.f;

	float FlapPitch = 0.f;

	float ElevatorRoll = 0.f;
	float ElevatorPitch = 0.f;

	static constexpr int InterpSpeed = 30;
};
