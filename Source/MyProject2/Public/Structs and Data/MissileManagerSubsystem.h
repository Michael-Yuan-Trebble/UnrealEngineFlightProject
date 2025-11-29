// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Weapons/Missiles/BaseMissile.h"
#include "MissileManagerSubsystem.generated.h"

UCLASS()
class MYPROJECT2_API UMissileManagerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	void RegisterMissile(ABaseMissile* InMissile);
	void UnregisterMissile(ABaseMissile* InMissile);

	TArray<TWeakObjectPtr<ABaseMissile>> ActiveMissiles;
};
