// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MissileAudioData.generated.h"

UCLASS()
class MYPROJECT2_API UMissileAudioData : public UDataAsset
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<USoundWave> MissileLaunchSound = nullptr;
};
