// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseIRMissile.h"
#include "R60.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT2_API AR60 : public ABaseIRMissile
{
	GENERATED_BODY()

public:

	AR60();

	float missileAcceleration = 1000.f;

	float missileVelocity = 0.f;

	float currentSpeed;

	float missileMaxSpeed = 1000.f;

	AActor* Tracking;

	float turnRate = 50.f;

	FRotator missileRotation = FRotator::ZeroRotator;

	virtual void FireStatic(float launchSpeed) override;

	virtual void FireTracking(float launchSpeed, AActor* Target) override;

	void LaunchSequence();

protected:

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;
};
