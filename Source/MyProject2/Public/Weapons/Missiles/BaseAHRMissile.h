// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "Weapons/Missiles/BaseMissile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Structs and Data/Weapon Data/ARHMissileStats.h"
#include "BaseAHRMissile.generated.h"

class ABaseAircraft;

UCLASS()
class MYPROJECT2_API ABaseAHRMissile : public ABaseMissile
{
	GENERATED_BODY()
	
public:
	ABaseAHRMissile();

	bool isDropPhase = false;

	float DropTimer = 0;

	UPROPERTY(EditAnywhere)
	UARHMissileStats* MissileStats = nullptr;

	float DropAcceleration = 40;

	float dropVelocity = 0;

	virtual void LaunchSequence(float Speed) override;

protected:

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void FireTracking(float launchSpeed, AActor* Target);

	virtual void FireStatic(float launchSpeed);

private:
	virtual void CheckAndDelete(AActor* OtherActor) override;
};
