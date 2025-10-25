// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Structs and Data/LockableTarget.h"
#include "Components/BoxComponent.h"
#include "Structs and Data/Aircraft Data/AircraftStats.h"
#include "Structs and Data/Weapon Data/BulletStats.h"
#include "Components/SkeletalMeshComponent.h"
#include "Structs and Data/DamageableInterface.h"
#include "Structs and Data/TeamInterface.h"
#include "BaseAircraft.generated.h"

class UFlightComponent;
class URadarComponent;
class UWeaponSystemComponent;

UCLASS()
class MYPROJECT2_API ABaseAircraft : public APawn, public ILockableTarget, public IDamageableInterface, public ITeamInterface
{
	GENERATED_BODY()

public:
	ABaseAircraft();

	UPROPERTY(EditAnywhere)
	EFaction Faction;

	UPROPERTY(EditAnywhere)
	UAircraftStats* AirStats;

	UPROPERTY(EditAnywhere)
	UBulletStats* BulletStats;

	UPROPERTY(BlueprintReadWrite)
	UFlightComponent* FlightComponent;

	UPROPERTY(BlueprintReadWrite)
	UWeaponSystemComponent* WeaponComponent;

	URadarComponent* RadarComponent;

	float springArmLength;

	float TakeoffSpeed;

	float InputPitchValue;

	float InputRollValue;

	float InputYawValue;

	float InputThrust;

	bool isAlive;

	float LockTime;

	bool bLocked;

	float health;

	AActor* Tracking;

	//UObjects

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMeshComponent* Airframe;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* Collision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* BodyCollision;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UBoxComponent* LeftWingCollision;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UBoxComponent* RightWingCollision;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UBoxComponent* RudderCollision;

	//Return Functions for UObjects

	virtual USkeletalMeshComponent* GetMesh() const { return Airframe; };

	virtual FVector GetTargetLocation() const override { return this->GetActorLocation(); };

	virtual bool IsLockable() const override { return isAlive; };

	//Return Functions for Vars

	virtual float ReturnTakeoffSpeed() const { return TakeoffSpeed; };

	virtual void PossessedBy(AController* Controller) override;

	virtual void OnHitByMissile_Implementation(AActor* Missile, float Damage) override;

	virtual EFaction GetFaction_Implementation() const override { return Faction; };

protected:

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaTime) override;

};
