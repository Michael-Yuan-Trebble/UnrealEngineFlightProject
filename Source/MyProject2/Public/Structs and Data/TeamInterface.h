// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "TeamInterface.generated.h"

UENUM(BlueprintType)
enum class EFaction : uint8 {
	Ally UMETA(DisplayName = "Ally"),
	Enemy UMETA(DisplayName = "Enemy")
};

UINTERFACE(MinimalAPI, Blueprintable)
class UTeamInterface : public UInterface
{
	GENERATED_BODY()
};

class MYPROJECT2_API ITeamInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Faction")
	EFaction GetFaction() const;
};