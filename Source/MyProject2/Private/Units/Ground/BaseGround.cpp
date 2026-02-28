// Fill out your copyright notice in the Description page of Project Settings.

#include "Units/Ground/BaseGround.h"
#include "Components/BoxComponent.h"

ABaseGround::ABaseGround()
{
	PrimaryActorTick.bCanEverTick = true;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(UnitRoot);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	UnitType = ETargetType::Ground;
	health = 100;
}

void ABaseGround::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseGround::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}