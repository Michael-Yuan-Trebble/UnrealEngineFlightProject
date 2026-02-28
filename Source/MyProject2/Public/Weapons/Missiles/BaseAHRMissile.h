// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapons/Missiles/BaseMissile.h"
#include "Structs and Data/Weapon Data/ARHMissileStats.h"
#include "BaseAHRMissile.generated.h"

class ABaseAircraft;
class UProjectileMovementComponent;

UCLASS()
class MYPROJECT2_API ABaseAHRMissile : public ABaseMissile
{
	GENERATED_BODY()
	
public:
	ABaseAHRMissile();

	virtual void LaunchSequence(float Speed) override;

private:

	bool isDropPhase = false;

	float DropTimer = 0;

	float DropAcceleration = 40;

	float dropVelocity = 0;

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void FireTracking(float launchSpeed, AActor* Target);

	virtual void FireStatic(float launchSpeed);

	static constexpr float DropTime = 0.2f;

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UARHMissileStats> MissileStats = nullptr;

	virtual void CheckAndDelete(AActor* OtherActor) override;
};
