// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "AircraftPlayerController.h"
#include "BaseIRMissile.h"
#include "CooldownWeapon.h"
#include "BaseAircraft.generated.h"

UCLASS()
class MYPROJECT2_API ABaseAircraft : public APawn
{
	GENERATED_BODY()

public:
	ABaseAircraft();

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName> PylonSockets;

	AAircraftPlayerController* Controlled;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class USkeletalMeshComponent* Airframe;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USpringArmComponent* SpringArm;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UCameraComponent* Camera;

	virtual void Tick(float DeltaTime) override;
	void EquipWeapons(const TArray<TSubclassOf<ABaseIRMissile>>& WeaponClasses);
	void FireWeapon(int WeaponIndex);
	void SelectWeapon(int WeaponIndex);
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FCooldownWeapon> AvailableWeapons;
	TArray<ABaseIRMissile*> CooldownWeapons;
	TMap<FName, TArray<ABaseIRMissile*>> AttachedWeaponsToHardpoints;
	TArray<FName> Pylons;
	ABaseIRMissile* CurrentWeapon;
	virtual float ReturnAcceleration() const;
	virtual float ReturnTakeoffSpeed() const;
	virtual float ReturnRollRate() const;
	virtual float ReturnTurnRate() const;
	virtual float ReturnMaxSpeed() const;
	virtual float ReturnThrust() const;
	virtual float ReturnRudder() const;
	virtual void AddPylons();
	virtual float ReturnSpringArmLength() const;
	virtual void ReEquip(FCooldownWeapon* Replace);
	virtual USpringArmComponent* GetSpringArm() const;
	virtual UCameraComponent* GetCamera() const;
	virtual USkeletalMeshComponent* GetMesh() const;

protected:
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* Controller) override;
};
