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
	TObjectPtr<UAudioComponent> PersonalAircraftAudio = nullptr;

	UPROPERTY()
	TObjectPtr<UAudioComponent> GunAudio = nullptr;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundWave> CockpitSound = nullptr;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundWave> ThirdPersonSound = nullptr;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundWave> GunSound = nullptr;

	UPROPERTY()
	TObjectPtr<ABaseAircraft> Controlled = nullptr;
	
	void PlayPerspectiveSound(const ECameraPerspective Perspective);

	void HandleGunSound(bool bFiring);

	void SetControlled(ABaseAircraft* InControl) { Controlled = InControl; };
};
