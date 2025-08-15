// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Structs and Data/LockableTarget.h"
#include "Structs and Data/CooldownWeapon.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/BoxComponent.h"
#include "Camera/CameraComponent.h"
#include "Structs and Data/AircraftStats.h"
#include "Components/SkeletalMeshComponent.h"
#include "BaseAircraft.generated.h"

class AAircraftPlayerController;
class ABaseIRMissile;
class UFlightComponent;

UCLASS()
class MYPROJECT2_API ABaseAircraft : public APawn, public ILockableTarget
{
	GENERATED_BODY()

public:
	ABaseAircraft();
	
	//Variables

	UFlightComponent* FlightComponent;

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

	AAircraftPlayerController* Controlled;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMeshComponent* Airframe;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USpringArmComponent* SpringArm;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCameraComponent* Camera;

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

	void SetStats(UAircraftStats* InStats);

	virtual USpringArmComponent* GetSpringArm() const;

	virtual UCameraComponent* GetCamera() const;

	virtual USkeletalMeshComponent* GetMesh() const;

	virtual FVector GetTargetLocation() const override;

	virtual bool IsLockable() const override;

	//Return Functions for Vars

	virtual float ReturnTakeoffSpeed() const;

	void AddPylons();

	void UpdateLockedOn(float DeltaSeconds);

	virtual float ReturnSpringArmLength() const;

	virtual void PossessedBy(AController* Controller) override;

protected:

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:

	virtual void ReEquip(FCooldownWeapon* Replace);

};
