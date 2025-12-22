// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "Structs and Data/Aircraft Data/AircraftStats.h"
#include "Structs and Data/Weapon Data/BulletStats.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "Components/SkeletalMeshComponent.h"
#include "BaseUnit.h"
#include "BaseAircraft.generated.h"

class UFlightComponent;
class URadarComponent;
class UWeaponSystemComponent;
class UAircraftVisualComponent;
class ABaseMissile;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMissileLaunchedAtSelf, ABaseMissile*, IncomingMissile);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLockedOnByEnemy);

UCLASS()
class MYPROJECT2_API ABaseAircraft : public ABaseUnit
{
	GENERATED_BODY()

public:
	ABaseAircraft();

	FOnMissileLaunchedAtSelf OnMissileLaunchedAtSelf;

	FOnLockedOnByEnemy OnLockedOnByEnemy;

	UPROPERTY(EditAnywhere)
	UAircraftStats* AirStats;

	UPROPERTY(EditAnywhere)
	UBulletStats* BulletStats;

	UPROPERTY(BlueprintReadWrite)
	UFlightComponent* FlightComponent;

	UPROPERTY(BlueprintReadWrite)
	UWeaponSystemComponent* WeaponComponent;

	URadarComponent* RadarComponent;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UAircraftVisualComponent> VisualCompClass;

	UPROPERTY(BlueprintReadOnly)
	UAircraftVisualComponent* VisualComp;

	float springArmLength;

	float TakeoffSpeed;

	float InputPitchValue;

	float InputRollValue;

	float InputYawValue;

	float InputThrust;

	float LockTime;

	bool bLocked;

	float AOA;

	UPROPERTY(EditAnywhere)
	int32 NumOfAfterburners;

	UPROPERTY(EditAnywhere)
	int32 NumOfVortices;

	UPROPERTY(EditANywhere)
	int32 NumOfMainWingVapors;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="VFX")
	TArray<UStaticMeshComponent*> AllMainWingVapors;

	UFUNCTION(BlueprintCallable, Category="Vapors")
	void SetWingVapors(const TArray<UStaticMeshComponent*>& Vapors) {
		AllMainWingVapors = Vapors;
	}

	AActor* Tracking;

	//UObjects

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMeshComponent* Airframe;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* BodyCollision;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UBoxComponent* LeftWingCollision;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UBoxComponent* RightWingCollision;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UBoxComponent* RudderCollision;

	UPROPERTY(EditAnywhere)
	UNiagaraSystem* AfterburnerSystem;

	TArray<UNiagaraComponent*> AllAfterburners;

	UPROPERTY(EditAnywhere)
	UNiagaraSystem* WingVortexSystem;

	TArray<UNiagaraComponent*> AllVortices;

	//Return Functions for UObjects

	virtual USkeletalMeshComponent* GetMesh() const { return Airframe; };

	//Return Functions for Vars

	virtual float ReturnTakeoffSpeed() const { return TakeoffSpeed; };

	virtual void PossessedBy(AController* Controller) override;

	virtual void HandleLOD(FVector CameraLoc) override;

	virtual void FireBullets() {};

	virtual void StartBullets() {};

	virtual void EndBullets() {};

public:
	// Set Controller
	void SetThrust(float thrust);

	void SetRoll(float roll);
	void SetPitch(float pitch);
	void SetRudder(float rudder);

	void SetFlying(bool bIsFlying);

	void FireWeaponSelected();

	void DisableAllMainWingVapors();
	void EnableAllMainWingVapors();

protected:

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	void ActivateAfterburnerFX();
	void DeactivateAfterburnerFX();

	void ActivateVortexFX();
	void DeactivateVortexFX();

private:

	UFUNCTION()
	void HandleAfterburnerFX(bool isActive);

	UFUNCTION()
	void HandleVortexFX(bool isActive);

	bool bIsVisible = true;
};
