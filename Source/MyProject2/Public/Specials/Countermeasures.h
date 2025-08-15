// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Specials/BaseSpecial.h"
#include "Aircraft/BaseAircraft.h"
#include "Countermeasures.generated.h"

UCLASS()
class MYPROJECT2_API UCountermeasures : public UBaseSpecial
{
	GENERATED_BODY()
	
public:
	virtual void ActivateSpecial() override;

private:
	TSubclassOf<AActor> FlareActorClass;
	ABaseAircraft* OwningAircraft;
};
