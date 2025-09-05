// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseIRMissile.h"
#include "Aim9.generated.h"

UCLASS()
class MYPROJECT2_API AAim9 : public ABaseIRMissile
{
	GENERATED_BODY()
	
public:
	AAim9();

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;
};
