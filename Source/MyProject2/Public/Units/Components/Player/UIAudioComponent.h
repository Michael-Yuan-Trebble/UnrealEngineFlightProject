// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Units/Components/Standard/BaseAudioComponent.h"
#include "Structs and Data/Aircraft Data/AircraftAudioData.h"
#include "UIAudioComponent.generated.h"

UCLASS()
class MYPROJECT2_API UUIAudioComponent : public UBaseAudioComponent
{
	GENERATED_BODY()
	
public:

	void LockingSound(bool bLocking);

	void LockedSound(bool bLocked);

	void SetAudio(UAircraftAudioData* InData);

private:
	UPROPERTY()
	TObjectPtr<USoundWave> CachedLockingOnSound = nullptr;

	UPROPERTY()
	TObjectPtr<USoundWave> CachedLockedOnSound = nullptr;

	UPROPERTY()
	TObjectPtr<UAudioComponent> LockingOn = nullptr;

	UPROPERTY()
	TObjectPtr<UAudioComponent> LockedOn = nullptr;
};
