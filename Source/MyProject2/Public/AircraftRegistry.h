// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "AircraftRegistry.generated.h"

class ABaseAircraft;

UCLASS()
class MYPROJECT2_API AAircraftRegistry : public AInfo
{
	GENERATED_BODY()

public:
	AAircraftRegistry();

	UPROPERTY(BlueprintReadOnly)
	TArray<ABaseAircraft*> RegisteredAircraft;

	void Register(ABaseAircraft* Aircraft);
	void Unregister(ABaseAircraft* Aircraft);

	static AAircraftRegistry* Get(UWorld* World);
};
