// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Structs and Data/LockableTarget.h"
#include "Structs and Data/CooldownWeapon.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/BoxComponent.h"
#include "Camera/CameraComponent.h"
#include "Structs and Data/Aircraft Data/AircraftStats.h"
#include "Components/SkeletalMeshComponent.h"
#include "BaseAircraft.generated.h"

class ABaseIRMissile;

UCLASS()
class MYPROJECT2_API ABaseAircraft : public APawn, public ILockableTarget
{
	GENERATED_BODY()

public:
	ABaseAircraft();
	
	//Variables

	UAircraftStats* AirStats;

	float springArmLength;

	float TakeoffSpeed;

	int NumPylons;

	float InputPitchValue;

	float InputRollValue;

	float InputYawValue;

	float InputThrust;

	bool isAlive;

	float LockTime;

	bool bLocked;

	AActor* Tracking;

	//UObjects

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName> PylonSockets;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMesh* Pylon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMeshComponent* Airframe;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* Collision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* BodyCollision;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UBoxComponent* LeftWingCollision;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UBoxComponent* RightWingCollision;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UBoxComponent* RudderCollision;

	//Arrays

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FCooldownWeapon> AvailableWeapons;

	ABaseIRMissile* CurrentWeapon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> Bullet;

	//Return Functions for UObjects

	virtual USkeletalMeshComponent* GetMesh() const;

	virtual FVector GetTargetLocation() const override;

	virtual bool IsLockable() const override;

	//Return Functions for Vars

	virtual float ReturnTakeoffSpeed() const;

	void AddPylons();

	void UpdateLockedOn(float DeltaSeconds);

	virtual void PossessedBy(AController* Controller) override;

protected:

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaTime) override;

};
