// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "BaseSpecial.generated.h"

UCLASS()
class MYPROJECT2_API UBaseSpecial : public UObject
{
	GENERATED_BODY()
	
public:
	FName SpecialName;

	virtual void ActivateSpecial() PURE_VIRTUAL(UBaseSpecial::ActivateSpecial, );
};
