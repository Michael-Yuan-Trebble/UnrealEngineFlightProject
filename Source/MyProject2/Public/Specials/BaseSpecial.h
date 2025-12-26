// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "BaseSpecial.generated.h"

UCLASS()
class MYPROJECT2_API UBaseSpecial : public UObject
{
	GENERATED_BODY()
	
public:
	FName SpecialName;

	float CooldownTime;

	float CurrentCooldown;

	void UpdateCooldown(float In) { CurrentCooldown = FMath::Clamp(CurrentCooldown - In, 0, CooldownTime); };

	virtual bool CanActivate() { return false; };

	virtual void ActivateSpecial() {};

	float GetCooldown() const { return CooldownTime; };
};
