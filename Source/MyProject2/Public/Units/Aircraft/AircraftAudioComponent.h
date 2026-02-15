// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Units/BaseAudioComponent.h"
#include "Enums/CameraPerspective.h"
#include "AircraftAudioComponent.generated.h"

class ABaseAircraft;

UCLASS()
class MYPROJECT2_API UAircraftAudioComponent : public UBaseAudioComponent
{
	GENERATED_BODY()
	
public:

	UAircraftAudioComponent();

	UPROPERTY()
	UAudioComponent* PersonalAircraftAudio = nullptr;

	UPROPERTY()
	UAudioComponent* GunAudio = nullptr;

	UPROPERTY(EditAnywhere)
	USoundWave* CockpitSound = nullptr;

	UPROPERTY(EditAnywhere)
	USoundWave* ThirdPersonSound = nullptr;

	UPROPERTY(EditAnywhere)
	USoundWave* GunSound = nullptr;

	UPROPERTY()
	ABaseAircraft* Controlled = nullptr;
	
	void PlayPerspectiveSound(const ECameraPerspective Perspective);

	void HandleGunSound(bool bFiring);

	void SetControlled(ABaseAircraft* InControl) { Controlled = InControl; };
};
