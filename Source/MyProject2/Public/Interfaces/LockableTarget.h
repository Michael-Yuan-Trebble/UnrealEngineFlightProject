// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Enums/TargetTypes.h"
#include "LockableTarget.generated.h"

UINTERFACE(BlueprintType)
class MYPROJECT2_API ULockableTarget : public UInterface
{
	GENERATED_BODY()
};

class MYPROJECT2_API ILockableTarget 
{

	GENERATED_BODY()

public: 

	virtual FVector GetTargetLocation() const = 0;
	virtual bool IsLockable() const = 0;
	virtual ETargetType GetTargetType() const = 0;
};