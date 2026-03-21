// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Units/Components/BaseWeaponAudioComponent.h"
#include "Structs and Data/MissileAudioData.h"
#include "MissileAudioComponent.generated.h"

UCLASS()
class MYPROJECT2_API UMissileAudioComponent : public UBaseWeaponAudioComponent
{
	GENERATED_BODY()
	
public:
	void SetAudio(UMissileAudioData* InAudio);

	virtual void Launch() override;

private:
	UPROPERTY()
	TObjectPtr<USoundWave> CachedMissileLaunch = nullptr;
};
