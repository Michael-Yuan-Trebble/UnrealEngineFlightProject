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
	bool bLandingGear = false;

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
	UBoxComponent* LandingGearCollision;

	UPROPERTY(EditAnywhere)
	UNiagaraSystem* AfterburnerSystem;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* LandingGear;

	TArray<UNiagaraComponent*> AllAfterburners;

	UPROPERTY(EditAnywhere)
	UNiagaraSystem* WingVortexSystem;

	TArray<UNiagaraComponent*> AllVortices;

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

	void SetLandingGearVisiblility(bool b) 
	{ 
		// TODO: For now its hardcoded for testing, but later change it so that the gamemode dictates if landing gear is present
		if (LandingGear) LandingGear->SetVisibility(bLandingGear);

		// TODO: Eventually have this collision box work, however it doesn't instantly kill the player upon reaching designated ground
		if (LandingGearCollision) LandingGearCollision->SetCollisionEnabled(bLandingGear ? ECollisionEnabled::NoCollision : ECollisionEnabled::NoCollision);
	};

	void SetSpeed(float speed);

	void FireWeaponSelected();

	void DisableAllMainWingVapors();
	void EnableAllMainWingVapors();

	USkeletalMeshComponent* GetMesh() const { return Airframe; };

	virtual float ReturnTakeoffSpeed() const { return TakeoffSpeed; };

	UFUNCTION(BlueprintCallable)
	float ReturnRudder() const;

	UFUNCTION(BlueprintCallable)
	float ReturnSlat() const;

	UFUNCTION(BlueprintCallable)
	float ReturnFlap() const;

	UFUNCTION(BlueprintCallable)
	float ReturnNozzle() const;

	UFUNCTION(BlueprintCallable)
	float ReturnAirbrake() const;

	UFUNCTION(BlueprintCallable)
	float ReturnElevator() const;

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
