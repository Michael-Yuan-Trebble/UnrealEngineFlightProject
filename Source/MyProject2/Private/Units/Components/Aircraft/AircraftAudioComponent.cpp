// Fill out your copyright notice in the Description page of Project Settings.

#include "Units/Components/Aircraft/AircraftAudioComponent.h"
#include "Sound/SoundWave.h"
#include "Sound/SoundAttenuation.h"
#include "Units/Aircraft/BaseAircraft.h"
#include "Units/Aircraft/Player/PlayerAircraft.h"
#include "Components/AudioComponent.h"

UAircraftAudioComponent::UAircraftAudioComponent() 
{

}

void UAircraftAudioComponent::SetAudio(UAircraftAudioData* InAudio) {
	if (!IsValid(InAudio)) return;
	CachedCockpit = InAudio->AircraftAudio.CockpitSound.LoadSynchronous();
	CachedThirdPerson = InAudio->AircraftAudio.ThirdPersonSound.LoadSynchronous();
	CachedGun = InAudio->GunAudios.GunSound.LoadSynchronous();
}

void UAircraftAudioComponent::PlayPerspectiveSound(const ECameraPerspective Perspective) 
{
	if (!PersonalAircraftAudio) 
	{
		if (AActor* Owner = GetOwner()) 
		{
			if (APlayerAircraft* Player = Cast<APlayerAircraft>(Owner)) 
				PersonalAircraftAudio = Player->GetAircraftAudio();
		}
	}
	if (!PersonalAircraftAudio) return;

	USoundWave* SoundToPlay = nullptr;
	switch (Perspective) 
	{
		case ECameraPerspective::ThirdPerson:
			SoundToPlay = CachedThirdPerson;
			break;
		case ECameraPerspective::FirstPerson:
			SoundToPlay = CachedCockpit;
			break;
		default:
			SoundToPlay = CachedThirdPerson;
			break;
	}

	if (!IsValid(SoundToPlay)) return;
	PersonalAircraftAudio->SetSound(SoundToPlay);
	if (!PersonalAircraftAudio->IsPlaying()) PersonalAircraftAudio->Play();
}

void UAircraftAudioComponent::HandleGunSound(bool bFiring) 
{
	if (!CachedGun) return;
	if (!GunAudio) 
	{
		if (AActor* Owner = GetOwner())
		{
			if (APlayerAircraft* Player = Cast<APlayerAircraft>(Owner))
			{
				if (GunAudio = Player->GetGunAudio()) 
					GunAudio->SetSound(CachedGun);
			}
		}
	}
	if (!GunAudio) return;
	if (bFiring && !GunAudio->IsPlaying()) 
	{
		GunAudio->Play();
	}
	else if (GunAudio->IsPlaying())
	{
		GunAudio->Stop();
	}
}