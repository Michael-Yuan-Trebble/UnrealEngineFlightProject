// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/BaseWeapon.h"
#include "BaseMissile.generated.h"

class ABaseAircraft;
class UProjectileMovementComponent;
class UNiagaraSystem;
class UNiagaraComponent;

UCLASS()
class MYPROJECT2_API ABaseMissile : public ABaseWeapon
{
	GENERATED_BODY()
	
public:

	ABaseMissile();

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UNiagaraSystem> SmokeTrailSystem = nullptr;

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UNiagaraSystem> MissileRocketSystem = nullptr;

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UNiagaraSystem> ExplosionEffect = nullptr;

	UPROPERTY()
	TWeakObjectPtr<UNiagaraComponent> MissileRocket = nullptr;

	UPROPERTY()
	TWeakObjectPtr<UNiagaraComponent> SmokeTrail = nullptr;

	UPROPERTY()
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement = nullptr;

	FVector CurrentDirection = FVector::ZeroVector;

	UPROPERTY()
	TWeakObjectPtr<AActor> Tracking = nullptr;

	UPROPERTY()
	TWeakObjectPtr<ABaseAircraft> AircraftOwner = nullptr;

	float missileAcceleration = 0.f;

	float missileMaxSpeed = 0.f;

	float missileVelocity = 0.f;

	float damage = 0.f;

	float turnRate = 0.f;

	float lifeTime = 0.f;

	bool bAir = false;

	bool bDestroyed = false;

	virtual void LaunchSequence(const float Speed);

	float ReturnCooldownTime() { return cooldownTime; };

	void activateSmoke();

	FTimerHandle VFXCheckhandle;

	void ApplyVFXLOD(const FVector& CameraDistance);

	void NotifyCountermeasure();

	bool bMissileVFXOn = true;

	bool bMissed = false;

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void DestroyMissile();

	virtual bool CalculateIfOvershoot(FVector ToTarget);

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

	virtual void CheckAndDelete(AActor* OtherActor);
};
