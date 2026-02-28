// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapons/Missiles/BaseMissile.h"
#include "Structs and Data/Weapon Data/IRMissileStats.h"
#include "BaseIRMissile.generated.h"

class ABaseAircraft;
class UProjectileMovementComponent;

UCLASS()
class MYPROJECT2_API ABaseIRMissile : public ABaseMissile
{
	GENERATED_BODY()
	
public:	
	ABaseIRMissile();

	virtual void LaunchSequence(float Speed) override;

private:

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UIRMissileStats> MissileStats = nullptr;

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void FireTracking(float launchSpeed, AActor* Target);

	virtual void FireStatic(float launchSpeed);

	virtual void CheckAndDelete(AActor* OtherActor) override;
};
