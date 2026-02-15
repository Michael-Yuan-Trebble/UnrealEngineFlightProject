// Fill out your copyright notice in the Description page of Project Settings.


#include "Units/Carrier.h"

ACarrier::ACarrier() {
	SpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("SpawnPoint"));
	SpawnPoint->SetupAttachment(RootComponent);

	CarrierMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CarrierMesh"));
	CarrierMesh->SetupAttachment(RootComponent);

	Collision = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
	Collision->SetupAttachment(RootComponent);
}