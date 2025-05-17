// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "BaseIRMissile.h"
#include "LockableTarget.h"
#include "CooldownWeapon.h"
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

	AActor* Tracking;

	FVector Velocity;

	//UObjects

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName> PylonSockets;

	AAircraftPlayerController* Controlled;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USkeletalMeshComponent* Airframe;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USpringArmComponent* SpringArm;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UCameraComponent* Camera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UBoxComponent* Collision;

	//Arrays

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FCooldownWeapon> AvailableWeapons;

	TArray<FName> Pylons;

	ABaseIRMissile* CurrentWeapon;

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
	
	//Stuff like drag, aoa, other physics based calculations

	float DragAOA(float AOA);

	void ReturnAOA(float DeltaSeconds);

	void AddPylons();

	void EquipWeapons(const TArray<TSubclassOf<ABaseIRMissile>>& WeaponClasses);

	void FireWeaponNotSelected(int WeaponIndex);

	void FireWeaponSelected(int WeaponIndex, AActor* Target);

	void SelectWeapon(int WeaponIndex);

	virtual float ReturnSpringArmLength() const;

protected:

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void PossessedBy(AController* Controller) override;

private:

	virtual void ReEquip(FCooldownWeapon* Replace);

};
