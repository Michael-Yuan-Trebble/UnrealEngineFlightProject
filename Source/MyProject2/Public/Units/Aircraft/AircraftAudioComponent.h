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
	UAudioComponent* PersonalAircraftAudio;

	UPROPERTY()
	UAudioComponent* GunAudio;

	UPROPERTY(EditAnywhere)
	USoundWave* CockpitSound;

	UPROPERTY(EditAnywhere)
	USoundWave* ThirdPersonSound;

	UPROPERTY(EditAnywhere)
	USoundWave* GunSound;

	UPROPERTY()
	ABaseAircraft* Controlled;
	
	void PlayPerspectiveSound(ECameraPerspective Perspective);

	void HandleGunSound(bool bFiring);

	void SetControlled(ABaseAircraft* InControl) { Controlled = InControl; };
};
