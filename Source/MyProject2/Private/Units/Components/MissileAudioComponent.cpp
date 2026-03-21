// Fill out your copyright notice in the Description page of Project Settings.


#include "Units/Components/MissileAudioComponent.h"
#include "Debug/DebugHelper.h"

void UMissileAudioComponent::SetAudio(UMissileAudioData* InAudio) {
	if (!IsValid(InAudio)) return;

	CachedMissileLaunch = InAudio->MissileLaunchSound.LoadSynchronous();
}

void UMissileAudioComponent::Launch() {
	LaunchSound(CachedMissileLaunch);
}