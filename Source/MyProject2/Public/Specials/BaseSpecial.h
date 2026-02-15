// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "BaseSpecial.generated.h"

class ABaseAircraft;

UCLASS(Abstract, Blueprintable, BlueprintType)
class MYPROJECT2_API UBaseSpecial : public UObject
{
	GENERATED_BODY()
	
public:
	FName SpecialName;

	UPROPERTY(EditAnywhere)
	float CooldownTime = 0.f;

	float CurrentCooldown = 0.f;

	void UpdateCooldown(const float In) { CurrentCooldown = FMath::Clamp(CurrentCooldown - In, 0, CooldownTime); };

	virtual bool CanActivate() { return CurrentCooldown == 0; };

	virtual void ActivateSpecial(ABaseAircraft* In) {};

	float GetCooldown() const { return CooldownTime; };
};
