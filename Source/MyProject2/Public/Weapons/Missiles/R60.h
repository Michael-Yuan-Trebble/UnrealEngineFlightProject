// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseIRMissile.h"
#include "R60.generated.h"

UCLASS()
class MYPROJECT2_API AR60 : public ABaseIRMissile
{
	GENERATED_BODY()

public:

	AR60();

protected:

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;
};
