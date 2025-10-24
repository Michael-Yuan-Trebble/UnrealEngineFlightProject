// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "Weapons/BaseWeapon.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Structs and Data/Weapon Data/ARHMissileStats.h"
#include "BaseAHRMissile.generated.h"

class ABaseAircraft;

UCLASS()
class MYPROJECT2_API ABaseAHRMissile : public ABaseWeapon
{
	GENERATED_BODY()
	
public:
	ABaseAHRMissile();

	bool isDropPhase = false;

	float DropTimer = 0;

	bool isAir = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UNiagaraSystem* SmokeTrailSystem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UNiagaraComponent* SmokeTrail;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UNiagaraSystem* MissileRocketSystem;

	UPROPERTY(EditAnywhere)
	UNiagaraComponent* MissileRocket;

	UPROPERTY(EditAnywhere)
	UARHMissileStats* MissileStats;

	UProjectileMovementComponent* ProjectileMovement;

	FVector CurrentDirection;

	AActor* Tracking;
	
	float missileAcceleration = 0;

	float missileMaxSpeed;

	float DropAcceleration = 40;

	float dropVelocity = 0;

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

	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void CheckAndDelete(AActor* OtherActor);

	void DestroyMissile();
};
