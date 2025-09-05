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

	bool isDropPhase;

	float DropTimer = 0;

	bool isAir;

	FVector CurrentDirection;

	AActor* Tracking;

	float missileAcceleration;

	float missileMaxSpeed;

	float missileVelocity;

	float turnRate;

	void LaunchSequence(float Speed);

	float ReturnCooldownTime();

protected:

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void FireTracking(float launchSpeed, AActor* Target);

	virtual void FireStatic(float launchSpeed);

};
