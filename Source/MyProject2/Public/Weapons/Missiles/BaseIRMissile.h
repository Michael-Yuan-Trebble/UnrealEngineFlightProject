// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapons/BaseWeapon.h"
#include "NiagaraFunctionLibrary.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Structs and Data/Weapon Data/IRMissileStats.h"
#include "NiagaraComponent.h"
#include "BaseIRMissile.generated.h"

class ABaseAircraft;

UCLASS()
class MYPROJECT2_API ABaseIRMissile : public ABaseWeapon
{
	GENERATED_BODY()
	
public:	
	ABaseIRMissile();

	bool isAir = false;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UNiagaraSystem* SmokeTrailSystem;

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	UNiagaraComponent* SmokeTrail;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UNiagaraSystem* MissileRocketSystem;

	UPROPERTY(EditAnywhere)
	UNiagaraComponent* MissileRocket;

	UPROPERTY(EditAnywhere)
	UIRMissileStats* MissileStats;

	UProjectileMovementComponent* ProjectileMovement;

	FVector CurrentDirection;

	AActor* Tracking;

	float missileAcceleration;

	float missileMaxSpeed;

	float missileVelocity;

	float turnRate;

	void LaunchSequence(float Speed);

	float ReturnCooldownTime();

	void activateSmoke();

	ABaseAircraft* Owner;

protected:

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void FireTracking(float launchSpeed, AActor* Target);

	virtual void FireStatic(float launchSpeed);

private:
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		FVector NormalImpulse,
		const FHitResult& Hit);

	void CheckAndDelete(AActor* OtherActor);

	void DestroyMissile();

};
