// Fill out your copyright notice in the Description page of Project Settings.


#include "Units/Naval/Carrier.h"
#include "Components/BoxComponent.h"

ACarrier::ACarrier() {
	SpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("SpawnPoint"));
	SpawnPoint->SetupAttachment(RootComponent);

	Collision = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
	Collision->SetupAttachment(RootComponent);
}