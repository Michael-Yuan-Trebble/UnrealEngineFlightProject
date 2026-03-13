// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Units/Components/Standard/BaseAudioComponent.h"
#include "Enums/CameraPerspective.h"
#include "Structs and Data/Aircraft Data/AircraftAudioData.h"
#include "Structs and Data/Audios/AircraftAudios.h"
#include "Structs and Data/Audios/GunAudios.h"
#include "AircraftAudioComponent.generated.h"

class ABaseAircraft;

UCLASS()
class MYPROJECT2_API UAircraftAudioComponent : public UBaseAudioComponent
{
	GENERATED_BODY()
	
public:

	UAircraftAudioComponent();
	
	void PlayPerspectiveSound(const ECameraPerspective Perspective);

	void HandleGunSound(bool bFiring);

	void SetControlled(ABaseAircraft* InControl) { Controlled = InControl; };

	void SetAudio(UAircraftAudioData* InAudio);

private:

	UPROPERTY()
	TObjectPtr<UAudioComponent> PersonalAircraftAudio = nullptr;

	UPROPERTY()
	TObjectPtr<UAudioComponent> GunAudio = nullptr;

	UPROPERTY()
	TObjectPtr<UAircraftAudioData> AudioData = nullptr;

	UPROPERTY()
	TObjectPtr<ABaseAircraft> Controlled = nullptr;

	UPROPERTY()
	TObjectPtr<USoundWave> CachedCockpit = nullptr;

	UPROPERTY()
	TObjectPtr<USoundWave> CachedThirdPerson = nullptr;

	UPROPERTY()
	TObjectPtr<USoundWave> CachedGun = nullptr;

};
