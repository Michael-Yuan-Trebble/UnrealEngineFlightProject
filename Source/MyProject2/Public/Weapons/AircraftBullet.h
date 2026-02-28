// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NiagaraSystem.h"
#include "Structs and Data/Weapon Data/BulletStats.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "AircraftBullet.generated.h"

class ABaseAircraft;

UCLASS()
class MYPROJECT2_API AAircraftBullet : public AActor
{
	GENERATED_BODY()
	
public:	
	AAircraftBullet();

protected:

	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMeshComponent> BulletMesh = nullptr;

	UPROPERTY(EditAnywhere)
	TObjectPtr<class UBoxComponent> Collision = nullptr;

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UBulletStats> BulletStat = nullptr;

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UNiagaraSystem> ExplosionEffect = nullptr;

	UPROPERTY()
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement = nullptr;

	UPROPERTY()
	TWeakObjectPtr<ABaseAircraft> AircraftOwner = nullptr;

	float BulletSpeed = 75000.f;

	float damage = 0.f;

	float LifeTime = 10.f;

	float currentTime = 0.f;

	virtual void BeginPlay() override;

	bool bDestroyed = false;

public:	

	void FireInDirection(const FVector& ShootDirection);

protected:
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	void DestroyBullet(AActor* OtherActor);

private:
	void EnableCollision();

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) {
		Super::EndPlay(EndPlayReason);
		GetWorldTimerManager().ClearAllTimersForObject(this);
	}
};
