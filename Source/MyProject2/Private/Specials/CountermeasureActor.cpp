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

	FlareComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("FlareEffect"));
	FlareComponent->SetupAttachment(RootComponent);
	FlareComponent->bAutoActivate = false;
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
	RootMesh->AddImpulse(GetActorForwardVector() * 100, NAME_None, true);
}