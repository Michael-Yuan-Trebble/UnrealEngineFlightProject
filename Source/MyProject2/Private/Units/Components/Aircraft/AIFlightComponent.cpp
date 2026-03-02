// Fill out your copyright notice in the Description page of Project Settings.


#include "Units/Aircraft/AI/AIFlightComponent.h"

UAIFlightComponent::UAIFlightComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UAIFlightComponent::BeginPlay()
{
	Super::BeginPlay();
	
}


// Called every frame
void UAIFlightComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

