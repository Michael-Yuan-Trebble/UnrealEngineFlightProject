// Fill out your copyright notice in the Description page of Project Settings.


#include "Units/Components/BaseWeaponAudioComponent.h"
#include "Components/AudioComponent.h"
#include "Debug/DebugHelper.h"

void UBaseWeaponAudioComponent::LaunchSound(USoundWave* InSound) {
	if (!IsValid(InSound)) return;
	if (!LaunchAudio) {
		if (AActor* Owner = GetOwner()) {
			LaunchAudio = CreateAndAttachAudioComp(Owner->GetRootComponent());
			LaunchAudio->SetSound(InSound);
		}
	}
	if (!LaunchAudio) return;
	if (!LaunchAudio->IsPlaying()) LaunchAudio->Play();
}

void UBaseWeaponAudioComponent::Launch() {

}