// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KillEvent.generated.h"

USTRUCT(BlueprintType)
struct FKillEvent
{
	GENERATED_BODY()

	UPROPERTY() TWeakObjectPtr<AActor> Killer;
	UPROPERTY() TWeakObjectPtr<AActor> Victim;
	UPROPERTY() TSubclassOf<AActor> WeaponUsed;
	UPROPERTY() float Time;
};
