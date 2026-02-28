// Fill out your copyright notice in the Description page of Project Settings.

#include "Units/Naval/BaseNaval.h"

ABaseNaval::ABaseNaval() {
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CarrierMesh"));
	Mesh->SetupAttachment(RootComponent);
}