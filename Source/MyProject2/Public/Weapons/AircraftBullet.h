// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Structs and Data/Weapon Data/BulletStats.h"
#include "AircraftBullet.generated.h"

UCLASS()
class MYPROJECT2_API AAircraftBullet : public AActor
{
	GENERATED_BODY()
	
public:	
	AAircraftBullet();

protected:

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* BulletMesh;

	UPROPERTY(EditAnywhere)
	class UBoxComponent* Collision;

	UPROPERTY(EditAnywhere)
	UBulletStats* BulletStat;

	float BulletSpeed = 100000.f;

	float LifeTime = 10.f;

	float currentTime = 0.f;

	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void FireInDirection(const FVector& ShootDirection);

protected:
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};
