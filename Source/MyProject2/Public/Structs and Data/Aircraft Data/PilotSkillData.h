// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PilotSkillData.generated.h"

UCLASS(BlueprintType)
class MYPROJECT2_API UPilotSkillData : public UDataAsset
{
	GENERATED_BODY()
public:
	// Used to determine how long (generally speaking) it takes for a unit to do something
	// Will make it random with the mean being this time, at least for now
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ReactionTime;

	// The amount of turn this AI can do, not entirely sure if this implementation or having advanced units use AOA delimiter instead
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ManueverSkill;

	// For enemies specifically, how often they target the Player, will be either 0 or -1 for friendly
	// Might make it so this comes into play with Friendly when targeting more HVT or make a general agression in order to make it more player vs HVT
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PlayerAggression;
};
