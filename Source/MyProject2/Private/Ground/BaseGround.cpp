// Fill out your copyright notice in the Description page of Project Settings.

#include "Ground/BaseGround.h"

ABaseGround::ABaseGround()
{
	PrimaryActorTick.bCanEverTick = true;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Collision);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	UnitType = ETargetType::Ground;
}

void ABaseGround::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseGround::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}