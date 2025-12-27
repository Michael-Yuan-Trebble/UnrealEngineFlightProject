// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Special!"));
#include "Units/Aircraft/SpecialSystemComponent.h"
#include "Specials/BaseSpecial.h"

USpecialSystemComponent::USpecialSystemComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void USpecialSystemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (IsValid(Special) && !Special->CanActivate()) Special->UpdateCooldown(DeltaTime);
}

void USpecialSystemComponent::SetSpecial(TSubclassOf<UBaseSpecial> In) {
	Special = NewObject<UBaseSpecial>(this, In);
}

void USpecialSystemComponent::ActivateSpecial(ABaseAircraft* In)
{
	if (Special && Special->CanActivate()) { Special->ActivateSpecial(In); }
}

void USpecialSystemComponent::DeactivateSpecial() {

}

