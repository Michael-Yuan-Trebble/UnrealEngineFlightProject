// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
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
	TObjectPtr<class UStaticMeshComponent> WeaponMesh = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<class UBoxComponent> Collision = nullptr;

	bool CanLockTarget(ETargetType TargetType) const { return SupportedTargetTypes.Contains(TargetType); }

	float timeTilDelt = 0.f;

	virtual void FireStatic(const float speed);

	virtual void FireTracking(const float speed, AActor* Target);

	const FName& GetName() const { return WeaponName; };

	const float& GetRange() const { return range; };

	const float& GetCooldown() const { return cooldownTime; };

	const bool& GetCanLock() const { return canLock; };

protected:

	bool canLock = false;

	float range = 0.f;

	float cooldownTime = 0.f;

	UPROPERTY(EditAnywhere)
	FName WeaponName = NAME_None;

	TArray<ETargetType> SupportedTargetTypes{};
	
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

};
