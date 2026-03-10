// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Structs and Data/AircraftAudios.h"
#include "Structs and Data/GunAudios.h"
#include "AircraftAudioData.generated.h"

UCLASS()
class MYPROJECT2_API UAircraftAudioData : public UDataAsset
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere)
	FAircraftAudios AircraftAudio{};

	UPROPERTY(EditAnywhere)
	FGunAudios GunAudios{};
};
