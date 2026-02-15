// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BaseAudioComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYPROJECT2_API UBaseAudioComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UBaseAudioComponent();

	UPROPERTY(VisibleAnywhere)
	UAudioComponent* PrimaryLoop = nullptr;

	void Init();

	void PlayLoop(USoundBase* Sound);
	void StopLoop();

	void PlayOneShot(USoundBase* Sound);
	void PlayOneShotAtLocation(USoundBase* Sound, FVector Location);

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category="Audio")
	float VolumeMultiplier = 1.f;

	UPROPERTY(EditAnywhere, Category="Audio")
	float PitchMultiplier = 1.f;

	bool bIsLooping = false;
	bool bIsInit = false;
};
