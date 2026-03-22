// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Structs and Data/Weapon Data/BaseWeaponStats.h"
#include "Interfaces/LockableTarget.h"
#include "BaseWeapon.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponResult, bool, bHit);

class UBoxComponent;

UCLASS()
class MYPROJECT2_API ABaseWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	
	UPROPERTY()
	FOnWeaponResult OnWeaponResult;

	ABaseWeapon();

	bool CanLockTarget(ETargetType TargetType) const;

	TArray<ETargetType> GetTargetTypes() const { return SupportedTargetTypes; };

	virtual void FireStatic(const float speed);

	virtual void FireTracking(const float speed, AActor* Target);

	virtual const FName& GetName() const { return WeaponName; };

	virtual const float& GetRange() const { return range; };

	virtual const float& GetCooldown() const { return cooldownTime; };

	virtual const bool& GetCanLock() const { return canLock; };

	UBoxComponent* GetCollision() const { return Collision; };

	virtual void LaunchAudio();

protected:

	bool canLock = false;

	float range = 0.f;

	float damage = 0.f;

	float lifeTime = 0.f;

	float cooldownTime = 0.f;

	float timeTilDelt = 0.f;

	UPROPERTY(EditAnywhere)
	FName WeaponName = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<class UStaticMeshComponent> WeaponMesh = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UBoxComponent> Collision = nullptr;

	TArray<ETargetType> SupportedTargetTypes{};
	
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

};
