// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapons/Missiles/BaseMissile.h"
#include "NiagaraFunctionLibrary.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Structs and Data/Weapon Data/IRMissileStats.h"
#include "NiagaraComponent.h"
#include "BaseIRMissile.generated.h"

class ABaseAircraft;

UCLASS()
class MYPROJECT2_API ABaseIRMissile : public ABaseMissile
{
	GENERATED_BODY()
	
public:	
	ABaseIRMissile();

	UPROPERTY(EditAnywhere)
	UIRMissileStats* MissileStats = nullptr;

	virtual void LaunchSequence(float Speed) override;

protected:

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void FireTracking(float launchSpeed, AActor* Target);

	virtual void FireStatic(float launchSpeed);

private:
	virtual void CheckAndDelete(AActor* OtherActor) override;
};
