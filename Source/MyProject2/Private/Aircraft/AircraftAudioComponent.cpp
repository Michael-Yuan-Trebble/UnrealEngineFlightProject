// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Sound Manager!"));
#include "Aircraft/AircraftAudioComponent.h"
#include "Sound/SoundWave.h"
#include "Sound/SoundAttenuation.h"
#include "Aircraft/BaseAircraft.h"
#include "Components/AudioComponent.h"

UAircraftAudioComponent::UAircraftAudioComponent() {

}

void UAircraftAudioComponent::PlayPerspectiveSound(ECameraPerspective Perspective) {
	if (!PersonalAircraftAudio) {
		if (AActor* Owner = GetOwner()) {
			PersonalAircraftAudio = Owner->FindComponentByClass<UAudioComponent>();
		}
		if (!PersonalAircraftAudio) return;
	}
	USoundWave* SoundToPlay = nullptr;
	switch (Perspective) {
		case ECameraPerspective::ThirdPerson:
			SoundToPlay = ThirdPersonSound;
		case ECameraPerspective::FirstPerson:
			SoundToPlay = CockpitSound;
		default:
			SoundToPlay = ThirdPersonSound;
	}

	if (!SoundToPlay) return;
	PersonalAircraftAudio->SetSound(SoundToPlay);
	if (!PersonalAircraftAudio->IsPlaying()) {
		PersonalAircraftAudio->Play();
	}
}