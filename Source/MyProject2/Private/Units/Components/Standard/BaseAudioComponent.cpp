// Fill out your copyright notice in the Description page of Project Settings.


#include "Units/Components/Standard/BaseAudioComponent.h"
#include "Sound/SoundBase.h"
#include "Sound/SoundAttenuation.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"

UBaseAudioComponent::UBaseAudioComponent() {}

void UBaseAudioComponent::BeginPlay() 
{
	Super::BeginPlay();
}

void UBaseAudioComponent::PlayOneShot(USoundBase* Sound)
{
	if (!GetOwner()) return;
	UGameplayStatics::PlaySoundAtLocation(
		this,
		Sound,
		GetOwner()->GetActorLocation()
	);
}

void UBaseAudioComponent::PlayOneShotAtLocation(USoundBase* Sound, FVector Location)
{
	UGameplayStatics::PlaySoundAtLocation(
		this,
		Sound,
		Location
	);
}

UAudioComponent* UBaseAudioComponent::CreateAndAttachAudioComp(USceneComponent* AttachTo) {
	AActor* Owner = GetOwner();
	if (!IsValid(Owner) || !IsValid(AttachTo)) return nullptr;

	UWorld* World = Owner->GetWorld();
	if (!IsValid(World)) return nullptr;

	UAudioComponent* AudioComp = NewObject<UAudioComponent>(Owner);
	if (!IsValid(AudioComp)) return nullptr;

	AudioComp->RegisterComponentWithWorld(World);
	AudioComp->AttachToComponent(AttachTo, FAttachmentTransformRules::KeepRelativeTransform);

	AudioComp->bAutoActivate = false;
	return AudioComp;
}