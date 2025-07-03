// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Weapons/Missiles/BaseIRMissile.h"
#include "Structs and Data/LockableTarget.h"
#include "Structs and Data/CooldownWeapon.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/BoxComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "BaseAircraft.generated.h"

class AAircraftPlayerController;

UCLASS()
class MYPROJECT2_API ABaseAircraft : public APawn, public ILockableTarget
{
	GENERATED_BODY()

public:
	ABaseAircraft();
	
	//Variables

	float springArmLength;

	float ListedAcceleration;

	float TakeoffSpeed;

	float RollRate;

	float ListedTurnRate;

	int ListedMaximumSpeed;

	float ListedThrust;

	float ListedRudder;

	int NumPylons;

	float InputPitchValue;

	float InputRollValue;

	float InputYawValue;

	float InputThrust;

	bool isAlive;

	float currentSpeed;
	
	float power;

	float NextRoll;

	float NextPitch;

	float NextYaw;

	float UserRoll;

	float UserPitch;

	float UserYaw;

	bool isFlying;

	float LockTime;

	bool bLocked;

	float AOAReturnSpeed;

	AActor* Tracking;

	FVector Velocity;

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

	virtual USpringArmComponent* GetSpringArm() const;

	virtual UCameraComponent* GetCamera() const;

	virtual USkeletalMeshComponent* GetMesh() const;

	virtual FVector GetTargetLocation() const override;

	virtual bool IsLockable() const override;

	//Return Functions for Vars

	virtual float ReturnAcceleration() const;

	virtual float ReturnTakeoffSpeed() const;

	virtual float ReturnRollRate() const;

	virtual float ReturnTurnRate() const;

	virtual float ReturnMaxSpeed() const;

	virtual float ReturnThrust() const;

	virtual float ReturnRudder() const;

	void ApplySpeed(float ThrottlePercent, float DeltaSeconds);
	
	void ApplyRot(float DeltaSeconds);

	void SetPitch(float PitchValue);

	void SetYaw(float YawValue);

	void SetRoll(float RollValue);

	void ApplyPitch(float DeltaSeconds);

	void ApplyYaw(float DeltaSeconds);

	void ApplyRoll(float DeltaSeconds);

	void SlowSpeed(float ThrottlePercentage);

	void NormalSpeed(float ThrottlePercentage);

	void AfterburnerSpeed(float ThrottlePercentage);

	void AdjustSpringArm(float DeltaSeconds, float ThrottlePercentage);
	
	//Stuff like drag, aoa, other physics based calculations

	float DragAOA(float AOA);

	void ReturnAOA(float DeltaSeconds);

	void AddPylons();

	void EquipWeapons(const TArray<TSubclassOf<ABaseIRMissile>>& WeaponClasses);

	void FireWeaponNotSelected(int WeaponIndex);

	void FireWeaponSelected(int WeaponIndex, AActor* Target);

	void SelectWeapon(int WeaponIndex);

	void FireBullets();

	void UpdateLockedOn(float DeltaSeconds);

	virtual float ReturnSpringArmLength() const;

protected:

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void PossessedBy(AController* Controller) override;

private:

	virtual void ReEquip(FCooldownWeapon* Replace);

};
