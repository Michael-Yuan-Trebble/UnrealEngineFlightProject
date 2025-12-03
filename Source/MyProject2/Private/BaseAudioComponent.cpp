// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseAudioComponent.h"
#include "Sound/SoundBase.h"
#include "Sound/SoundAttenuation.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"

UBaseAudioComponent::UBaseAudioComponent() {}

void UBaseAudioComponent::BeginPlay() {
	Super::BeginPlay();
	if (!PrimaryLoop) 
	{
		PrimaryLoop = NewObject<UAudioComponent>(GetOwner(), TEXT("PrimaryLoop"));
		if (PrimaryLoop) 
		{
			USceneComponent* OwnerRoot = GetOwner()->GetRootComponent();
			PrimaryLoop->AttachToComponent(OwnerRoot, FAttachmentTransformRules::KeepRelativeTransform);
			PrimaryLoop->bAutoActivate = false;
			PrimaryLoop->RegisterComponent();
		}
	}
}

void UBaseAudioComponent::PlayLoop(USoundBase* Sound) {
	if (!PrimaryLoop || !Sound) return;
	PrimaryLoop->SetSound(Sound);
	PrimaryLoop->Play();
}

void UBaseAudioComponent::StopLoop() {
	if (!PrimaryLoop) return;
	PrimaryLoop->Stop();
}

void UBaseAudioComponent::PlayOneShot(USoundBase* Sound) {
	if (!GetOwner()) return;
	UGameplayStatics::PlaySoundAtLocation(
		this,
		Sound,
		GetOwner()->GetActorLocation()
	);
}

void UBaseAudioComponent::PlayOneShotAtLocation(USoundBase* Sound, FVector Location) {
	UGameplayStatics::PlaySoundAtLocation(
		this,
		Sound,
		Location
	);
}