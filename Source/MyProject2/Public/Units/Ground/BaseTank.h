// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Units/Ground/BaseGround.h"
#include "BaseTank.generated.h"

UCLASS()
class MYPROJECT2_API ABaseTank : public ABaseGround
{
	GENERATED_BODY()
	
public:
	ABaseTank();

	virtual void PossessedBy(AController* Controller) override;

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;
};
