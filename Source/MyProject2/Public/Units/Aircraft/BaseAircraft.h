// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "Structs and Data/Aircraft Data/AircraftStats.h"
#include "Structs and Data/Weapon Data/BulletStats.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "Interfaces/ApproachingMissileInterface.h"
#include "Components/SkeletalMeshComponent.h"
#include "Units/BaseUnit.h"
#include "Weapons/Missiles/BaseMissile.h"
#include "BaseAircraft.generated.h"

class UFlightComponent;
class URadarComponent;
class UWeaponSystemComponent;
class UAircraftVisualComponent;
class USpecialSystemComponent;
class ABaseWeapon;
class UBaseSpecial;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMissileLaunchedAtSelf, ABaseMissile*, IncomingMissile);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLockedOnByEnemy);

UCLASS()
class MYPROJECT2_API ABaseAircraft : public ABaseUnit, public IApproachingMissileInterface
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

	UPROPERTY()
	URadarComponent* RadarComponent;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UAircraftVisualComponent> VisualCompClass;

	UPROPERTY(BlueprintReadOnly)
	UAircraftVisualComponent* VisualComp;

	UPROPERTY()
	USpecialSystemComponent* SpecialComp;

	TArray<TWeakObjectPtr<ABaseMissile>> IncomingMissiles;

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
	void SetWingVapors(const TArray<UStaticMeshComponent*>& Vapors) { AllMainWingVapors = Vapors; }

	UPROPERTY()
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

	UPROPERTY()
	TArray<UNiagaraComponent*> AllAfterburners;

	UPROPERTY(EditAnywhere)
	UNiagaraSystem* WingVortexSystem;

	UPROPERTY()
	TArray<UNiagaraComponent*> AllVortices;

	virtual void PossessedBy(AController* Controller) override;

	virtual void HandleLOD(FVector CameraLoc) override;

	virtual void FireBullets() {};

	virtual void StartBullets() {};

	virtual void EndBullets() {};

	void ActivateSpecial();

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

	void SetWeapons(TMap<FName, TSubclassOf<ABaseWeapon>> In);

	void SetSpecial(TSubclassOf<UBaseSpecial> In);

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
	float ReturnRFlap() const;

	UFUNCTION(BlueprintCallable)
	float ReturnLFlap() const;

	UFUNCTION(BlueprintCallable)
	float ReturnNozzle() const;

	UFUNCTION(BlueprintCallable)
	float ReturnAirbrake() const;

	UFUNCTION(BlueprintCallable)
	float ReturnElevator() const;

	virtual void RegisterIncomingMissile_Implementation(ABaseMissile* Missile) override {
		IncomingMissiles.AddUnique(Missile);
	};

	virtual void UnregisterIncomingMissile_Implementation(ABaseMissile* Missile) override {
		IncomingMissiles.RemoveSingle(Missile);
	};

	virtual void OnCountermeasureDeployed_Implementation() override;

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
