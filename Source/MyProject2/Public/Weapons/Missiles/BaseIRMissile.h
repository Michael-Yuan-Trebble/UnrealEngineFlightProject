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

	AActor* Tracking;

	float missileAcceleration;

	float missileMaxSpeed;

	float turnRate;

	void FireStatic(float launchSpeed);

	void FireTracking(float launchSpeed, AActor* Target);

	void LaunchSequence();

	float ReturnCooldownTime();

protected:

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

};
