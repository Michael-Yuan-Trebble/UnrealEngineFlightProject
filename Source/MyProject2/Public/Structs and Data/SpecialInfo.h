// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Specials/BaseSpecial.h"
#include "SpecialInfo.generated.h"

USTRUCT(BlueprintType)
struct MYPROJECT2_API FSpecialInfo
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FName SpecialName = NAME_None;
};
