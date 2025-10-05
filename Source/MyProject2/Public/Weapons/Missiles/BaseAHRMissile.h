// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "Weapons/BaseWeapon.h"
#include "Structs and Data/Weapon Data/ARHMissileStats.h"
#include "BaseAHRMissile.generated.h"

UCLASS()
class MYPROJECT2_API ABaseAHRMissile : public ABaseWeapon
{
	GENERATED_BODY()
	
public:
	ABaseAHRMissile();

	bool isDropPhase;

	float DropTimer = 0;

	bool isAir;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UNiagaraSystem* SmokeTrailSystem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UNiagaraComponent* SmokeTrail;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UARHMissileStats* MissileStats;

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

private:
	float calculatePitchAngle();

	float calculateYawAngle();
};
