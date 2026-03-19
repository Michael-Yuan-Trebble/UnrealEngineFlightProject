// Fill out your copyright notice in the Description page of Project Settings.


#include "Units/Components/Player/UIAudioComponent.h"
#include "Structs and Data/Audios/UIAudios.h"
#include "Units/Aircraft/Player/PlayerAircraft.h"
#include "Components/AudioComponent.h"
#include "Debug/DebugHelper.h"

void UUIAudioComponent::SetAudio(UAircraftAudioData* InData) {
	if (!IsValid(InData)) return;
	CachedLockingOnSound = InData->UIAudios.LockingSound.LoadSynchronous();
	CachedLockedOnSound = InData->UIAudios.LockedSound.LoadSynchronous();
}

void UUIAudioComponent::LockingSound(bool bLocking) {
	if (!CachedLockingOnSound) return;
	if (!LockingOn) {
		if (AActor* Owner = GetOwner())
		{
			LockingOn = CreateAndAttachAudioComp(Owner->GetRootComponent());
			if (!LockingOn) return;
			LockingOn->SetSound(CachedLockingOnSound);
		}
	}
	if (bLocking && !LockingOn->IsPlaying()) {
		LockingOn->Play();
	}
	else if (!bLocking && LockingOn->IsPlaying()) {
		LockingOn->Stop();
	}
}

void UUIAudioComponent::LockedSound(bool bLocked) {
	if (!CachedLockedOnSound) return;
	if (!LockedOn) {
		if (AActor* Owner = GetOwner()) {
			LockedOn = CreateAndAttachAudioComp(Owner->GetRootComponent());
			if (!LockedOn) return;
			LockedOn->SetSound(CachedLockedOnSound);
		}
	}
	if (bLocked && !LockedOn->IsPlaying()) {
		LockedOn->Play();
		if (LockingOn && LockingOn->IsPlaying()) LockingOn->Stop();
	}
	else if (!bLocked && LockedOn->IsPlaying()) {
		LockedOn->Stop();
	}
}