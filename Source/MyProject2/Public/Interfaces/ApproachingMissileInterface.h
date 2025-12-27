// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ApproachingMissileInterface.generated.h"

class ABaseMissile;

UINTERFACE(MinimalAPI, Blueprintable)
class UApproachingMissileInterface : public UInterface {
	GENERATED_BODY()
};

class MYPROJECT2_API IApproachingMissileInterface
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintNativeEvent)
	void RegisterIncomingMissile(ABaseMissile* Missile);

	UFUNCTION(BlueprintNativeEvent)
	void UnregisterIncomingMissile(ABaseMissile* Missile);

	UFUNCTION(BlueprintNativeEvent)
	void OnCountermeasureDeployed();
};
