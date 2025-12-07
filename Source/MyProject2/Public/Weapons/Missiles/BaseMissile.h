// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/BaseWeapon.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "BaseMissile.generated.h"

class ABaseAircraft;

UCLASS()
class MYPROJECT2_API ABaseMissile : public ABaseWeapon
{
	GENERATED_BODY()
	
public:

	ABaseMissile();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UNiagaraSystem* SmokeTrailSystem;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UNiagaraComponent* SmokeTrail;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UNiagaraSystem* MissileRocketSystem;

	UPROPERTY(EditAnywhere)
	UNiagaraComponent* MissileRocket;

	UPROPERTY(EditAnywhere)
	UNiagaraSystem* ExplosionEffect;

	UProjectileMovementComponent* ProjectileMovement;

	FVector CurrentDirection;

	AActor* Tracking;

	float missileAcceleration;

	float missileMaxSpeed;

	float missileVelocity;

	float turnRate;

	bool bAir = false;

	bool bDestroyed = false;

	virtual void LaunchSequence(float Speed);

	float ReturnCooldownTime() { return cooldownTime; };

	void activateSmoke();

	ABaseAircraft* Owner;

	FTimerHandle VFXCheckhandle;

	void ApplyVFXLOD(FVector CameraDistance);

	bool bMissileVFXOn = true;

	bool bMissed = false;

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

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
