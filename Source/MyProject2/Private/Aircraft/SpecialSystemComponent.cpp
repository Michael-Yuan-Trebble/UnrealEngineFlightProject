// Fill out your copyright notice in the Description page of Project Settings.


#include "Aircraft/SpecialSystemComponent.h"
#include "Specials/BaseSpecial.h"

USpecialSystemComponent::USpecialSystemComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void USpecialSystemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (Special && !Special->CanActivate()) Special->UpdateCooldown(DeltaTime);
}

void USpecialSystemComponent::Activate() 
{
	if (Special && Special->CanActivate()) Special->ActivateSpecial();
}

void USpecialSystemComponent::Deactivate() {

}

