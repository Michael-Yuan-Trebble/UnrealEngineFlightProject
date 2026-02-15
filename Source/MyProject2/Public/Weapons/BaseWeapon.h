// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "NiagaraSystem.h"
#include "Components/StaticMeshComponent.h"
#include "Structs and Data/Weapon Data/BaseWeaponStats.h"
#include "Interfaces/LockableTarget.h"
#include "BaseWeapon.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponResult, bool, bHit);

UCLASS()
class MYPROJECT2_API ABaseWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	
	UPROPERTY()
	FOnWeaponResult OnWeaponResult;

	ABaseWeapon();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* WeaponMesh = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* Collision = nullptr;

	TArray<ETargetType> SupportedTargetTypes;

	bool CanLockTarget(ETargetType TargetType) const { return SupportedTargetTypes.Contains(TargetType); }

	UPROPERTY(EditAnywhere)
	FName WeaponName;

	float cooldownTime = 0.f;

	float timeTilDelt = 0.f;

	bool canLock = false;

	float range = 0.f;

	virtual void FireStatic(const float speed);

	virtual void FireTracking(const float speed, AActor* Target);

protected:
	
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

};
