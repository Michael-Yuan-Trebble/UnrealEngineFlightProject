// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Structs and Data/Weapon Data/BaseWeaponStats.h"
#include "Structs and Data/LockableTarget.h"
#include "BaseWeapon.generated.h"

UCLASS()
class MYPROJECT2_API ABaseWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	
	ABaseWeapon();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* WeaponMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* Collision;

	TArray<ETargetType> SupportedTargetTypes;

	bool CanLockTarget(ETargetType TargetType) const { return SupportedTargetTypes.Contains(TargetType); }

	FName WeaponName;

	float cooldownTime;

	float timeTilDelt = 0;

	bool canLock;

	float range;

	virtual void FireStatic(float speed);

	virtual void FireTracking(float speed, AActor* Target);

protected:
	
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

};
