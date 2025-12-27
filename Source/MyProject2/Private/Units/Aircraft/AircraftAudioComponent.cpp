// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Sound Manager!"));
#include "Units/Aircraft/AircraftAudioComponent.h"
#include "Sound/SoundWave.h"
#include "Sound/SoundAttenuation.h"
#include "Units/Aircraft/BaseAircraft.h"
#include "Units/Aircraft/Player/PlayerAircraft.h"
#include "Components/AudioComponent.h"

UAircraftAudioComponent::UAircraftAudioComponent() 
{

}

void UAircraftAudioComponent::PlayPerspectiveSound(ECameraPerspective Perspective) 
{
	if (!PersonalAircraftAudio) 
	{
		if (AActor* Owner = GetOwner()) 
		{
			if (APlayerAircraft* Player = Cast<APlayerAircraft>(Owner)) 
			{
				PersonalAircraftAudio = Player->GetAircraftAudio();
			}
		}
		if (!PersonalAircraftAudio) return;
	}
	USoundWave* SoundToPlay = nullptr;
	switch (Perspective) 
	{
		case ECameraPerspective::ThirdPerson:
			SoundToPlay = ThirdPersonSound;
			break;
		case ECameraPerspective::FirstPerson:
			SoundToPlay = CockpitSound;
			break;
		default:
			SoundToPlay = ThirdPersonSound;
			break;
	}

	if (!SoundToPlay) return;
	PersonalAircraftAudio->SetSound(SoundToPlay);
	if (!PersonalAircraftAudio->IsPlaying()) PersonalAircraftAudio->Play();
}

void UAircraftAudioComponent::HandleGunSound(bool bFiring) 
{
	if (!GunAudio) 
	{
		if (AActor* Owner = GetOwner())
		{
			if (APlayerAircraft* Player = Cast<APlayerAircraft>(Owner))
			{
				GunAudio = Player->GetGunAudio();
			}
		}
		if (!GunAudio || !GunSound) return;
		GunAudio->SetSound(GunSound);
	}
	if (bFiring) 
	{
		if (!GunAudio->IsPlaying()) GunAudio->Play();
	}
	else 
	{
		if (GunAudio->IsPlaying()) GunAudio->Stop();
	}
}