// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Weapons/BaseWeapon.h"
#include "Specials/BaseSpecial.h"
#include "Structs and Data/Aircraft Data/AircraftData.h"
#include "CurrentPlayerState.generated.h"

UCLASS()
class MYPROJECT2_API ACurrentPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	ACurrentPlayerState();
};
