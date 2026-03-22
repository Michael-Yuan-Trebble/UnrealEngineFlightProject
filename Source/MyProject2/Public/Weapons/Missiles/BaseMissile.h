// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/BaseWeapon.h"
#include "Structs and Data/InGame/InGameMissileStats.h"
#include "BaseMissile.generated.h"

class ABaseAircraft;
class UProjectileMovementComponent;
class UNiagaraSystem;
class UNiagaraComponent;
class UMissileAudioComponent;

UCLASS()
class MYPROJECT2_API ABaseMissile : public ABaseWeapon
{
	GENERATED_BODY()
	
public:

	ABaseMissile();

	virtual void LaunchSequence(const float Speed);

	virtual void LaunchAudio() override;

	float ReturnCooldownTime() { return cooldownTime; };

	void activateSmoke();

	void ApplyVFXLOD(const FVector& CameraDistance);

	void NotifyCountermeasure();

	void SetAudio();

protected:

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

	UPROPERTY()
	TWeakObjectPtr<AActor> Tracking = nullptr;

	UPROPERTY()
	TWeakObjectPtr<ABaseAircraft> AircraftOwner = nullptr;

	UPROPERTY()
	UMissileAudioComponent* MissileAudioComp = nullptr;

	FInGameMissileStats InGameStats{};

	FVector CurrentDirection = FVector::ZeroVector;

	FTimerHandle VFXCheckhandle{};

	float LockOnRange = 0.f;

	float missileVelocity = 0.f;

	float PreviousDistance = 0.f;

	bool bMissileVFXOn = true;

	bool bMissed = false;

	bool bAir = false;

	bool bDestroyed = false;
	
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void DestroyMissile();

	virtual bool CalculateIfOvershoot(FVector ToTarget);

	void TurnTowardTarget(float Delta);

	UNiagaraComponent* CreateEffect(const FVector& Location, const FRotator& Rotation, UNiagaraSystem* System);

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
