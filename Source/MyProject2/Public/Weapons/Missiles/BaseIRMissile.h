// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapons/BaseWeapon.h"
#include "TimerManager.h"
#include "BaseIRMissile.generated.h"

UCLASS()
class MYPROJECT2_API ABaseIRMissile : public ABaseWeapon
{
	GENERATED_BODY()
	
public:	
	ABaseIRMissile();

	float range;

	bool isAir;

	virtual void FireStatic(float launchSpeed);

	virtual void FireTracking(float launchSpeed, AActor* Target);

protected:

	virtual void BeginPlay() override;

public:	

	virtual void Tick(float DeltaTime) override;

	float ReturnCooldownTime();

};
