// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Specials/BaseSpecial.h"
#include "Aircraft/BaseAircraft.h"
#include "Countermeasures.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFlareDeployed);

UCLASS()
class MYPROJECT2_API UCountermeasures : public UBaseSpecial
{
	GENERATED_BODY()
	
public:
	FOnFlareDeployed OnFlareDeployed;

	virtual void ActivateSpecial() override;

	virtual bool CanActivate() override;

private:
	
	UPROPERTY(EditAnywhere)
	float CooldownTime = 5.f;

	float CurrentCooldownTime = 0.f;
};
