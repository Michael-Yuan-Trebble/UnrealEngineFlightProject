// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KillEvent.generated.h"

USTRUCT(BlueprintType)
struct FKillEvent
{
	GENERATED_BODY()

	UPROPERTY() TWeakObjectPtr<AActor> Killer = nullptr;
	UPROPERTY() TWeakObjectPtr<AActor> Victim = nullptr;
	UPROPERTY() TSubclassOf<AActor> WeaponUsed = nullptr;
	UPROPERTY() float Time = 0.f;
};
