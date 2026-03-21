// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Units/Components/Standard/BaseAudioComponent.h"
#include "Structs and Data/MissileAudioData.h"
#include "BaseWeaponAudioComponent.generated.h"

UCLASS()
class MYPROJECT2_API UBaseWeaponAudioComponent : public UBaseAudioComponent
{
	GENERATED_BODY()

public:

	virtual void LaunchSound(USoundWave* InSound);

	virtual void Launch();

private:

	TObjectPtr<UAudioComponent> LaunchAudio = nullptr;
};
