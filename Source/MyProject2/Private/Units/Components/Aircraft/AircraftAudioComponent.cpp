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
	CachedThirdPersonAfterburner = InAudio->AircraftAudio.ThirdPersonAfterburner.LoadSynchronous();
	CachedGun = InAudio->GunAudios.GunSound.LoadSynchronous();
}

void UAircraftAudioComponent::PlayPerspectiveSound(const ECameraPerspective Perspective) 
{
	CurrentPerspective = Perspective;

	if (!PersonalAircraftAudio) 
	{
		if (AActor* Owner = GetOwner()) 
			PersonalAircraftAudio = CreateAndAttachAudioComp(Owner->GetRootComponent());
	}
	if (!PersonalAircraftAudio) return;

	USoundWave* SoundToPlay = nullptr;
	switch (Perspective) 
	{
		case ECameraPerspective::ThirdPerson:
			SoundToPlay = bAfterburnerActive ? CachedThirdPersonAfterburner : CachedThirdPerson;
			break;
		case ECameraPerspective::FirstPerson:
			SoundToPlay = CachedCockpit;
			break;
		default:
			SoundToPlay = bAfterburnerActive ? CachedThirdPersonAfterburner : CachedThirdPerson;
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
			GunAudio = CreateAndAttachAudioComp(Owner->GetRootComponent());
			if (!GunAudio) return;
			GunAudio->SetSound(CachedGun);
		}
	}
	if (bFiring && !GunAudio->IsPlaying()) 
	{
		GunAudio->Play();
	}
	else if (GunAudio->IsPlaying())
	{
		GunAudio->Stop();
	}
}

void UAircraftAudioComponent::HandleAfterburner(bool bActive) {
	if (bAfterburnerActive == bActive) return;
	bAfterburnerActive = bActive;
	PlayPerspectiveSound(CurrentPerspective);
}