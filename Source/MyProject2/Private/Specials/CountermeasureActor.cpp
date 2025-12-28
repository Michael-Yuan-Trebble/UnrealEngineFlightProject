// Fill out your copyright notice in the Description page of Project Settings.


#include "Specials/CountermeasureActor.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"

ACountermeasureActor::ACountermeasureActor()
{
	PrimaryActorTick.bCanEverTick = true;
	RootMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RootMesh"));
	RootComponent = RootMesh;

	RootMesh->SetSimulatePhysics(true);
	RootMesh->SetEnableGravity(true);
	RootMesh->SetLinearDamping(1.5f);
	RootMesh->SetAngularDamping(0.5f);
	RootMesh->SetMobility(EComponentMobility::Movable);

	FlareComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("FlareEffect"));
	FlareComponent->SetupAttachment(RootComponent);
	FlareComponent->SetAutoActivate(false);
	FlareComponent->SetUsingAbsoluteScale(true);
	FlareComponent->SetVisibility(true);
	FlareComponent->SetMobility(EComponentMobility::Movable);
}

void ACountermeasureActor::BeginPlay()
{
	Super::BeginPlay();
	
	if (!FlareEffect || !FlareComponent) return;
	FlareComponent->SetAsset(FlareEffect);
}

void ACountermeasureActor::Activate() 
{
	FlareComponent->Activate(true);
	RootMesh->AddImpulse(GetActorForwardVector() * 5, NAME_None, true);
}