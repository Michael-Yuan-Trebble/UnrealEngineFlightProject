// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"
#include "Structs and Data/Settings/UserAudioSettings.h"
#include "Structs and Data/Settings/UserCameraSettings.h"
#include "PlayerGameUserSettings.generated.h"

UCLASS(Config=Game)
class MYPROJECT2_API UPlayerGameUserSettings : public UGameUserSettings
{
	GENERATED_BODY()
	
public:
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly)
	FUserCameraSettings Camera{};

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly)
	FUserAudioSettings Audio{};

	void SetCameraSensitivity(float Value) {
		Camera.Sensitivity = Value;
		//SaveSettings();
	}

	void SetVolume(float Value) {
		Audio.Volume = Value;
		//SaveSettings();
	}
};
