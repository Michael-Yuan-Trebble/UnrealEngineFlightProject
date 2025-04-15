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

	//UObjects

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName> PylonSockets;

	AAircraftPlayerController* Controlled;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class USkeletalMeshComponent* Airframe;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USpringArmComponent* SpringArm;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UCameraComponent* Camera;

	//Arrays

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FCooldownWeapon> AvailableWeapons;

	TArray<FName> Pylons;

	ABaseIRMissile* CurrentWeapon;

	//Return Functions for UObjects

	virtual USpringArmComponent* GetSpringArm() const;

	virtual UCameraComponent* GetCamera() const;

	virtual USkeletalMeshComponent* GetMesh() const;

	//Return Functions for Vars

	virtual float ReturnAcceleration() const;

	virtual float ReturnTakeoffSpeed() const;

	virtual float ReturnRollRate() const;

	virtual float ReturnTurnRate() const;

	virtual float ReturnMaxSpeed() const;

	virtual float ReturnThrust() const;

	virtual float ReturnRudder() const;

	void AddPylons();

	void EquipWeapons(const TArray<TSubclassOf<ABaseIRMissile>>& WeaponClasses);

	void FireWeapon(int WeaponIndex);

	void SelectWeapon(int WeaponIndex);

	virtual float ReturnSpringArmLength() const;

protected:

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void PossessedBy(AController* Controller) override;

private:

	virtual void ReEquip(FCooldownWeapon* Replace);

};
