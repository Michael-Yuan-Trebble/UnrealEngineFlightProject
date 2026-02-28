// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Structs and Data/Aircraft Data/AircraftStats.h"
#include "Structs and Data/Weapon Data/BulletStats.h"
#include "Interfaces/ApproachingMissileInterface.h"
#include "Enums/FlightMode.h"
#include "Structs and Data/AircraftAnimationValues.h"
#include "Units/BaseUnit.h"
#include "Weapons/Missiles/BaseMissile.h"
#include "Enums/ThrottleStage.h"
#include "BaseAircraft.generated.h"

class UFlightComponent;
class URadarComponent;
class UWeaponSystemComponent;
class UAircraftVisualComponent;
class USpecialSystemComponent;
class UBaseSpecial;
class ABaseMissile;
class UNiagaraComponent;
class UNiagaraSystem;
class USkeletalMeshComponent;
class UBoxComponent;

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

	UFUNCTION(BlueprintCallable, Category="Vapors")
	void SetWingVapors(const TArray<UStaticMeshComponent*>& Vapors) { AllMainWingVapors = Vapors; }

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

	void SetRestrained(bool bIsRestrained);

	void SetLandingGearVisiblility(bool b);

	void SetSpeed(float speed);

	void SetWeapons(TMap<FName, TSubclassOf<ABaseWeapon>> In);

	void SetSpecial(TSubclassOf<UBaseSpecial> In);

	void SetFlightMode(EFlightMode FlightMode);

	void FireWeaponSelected();

	void DisableAllMainWingVapors();
	void EnableAllMainWingVapors();

	USkeletalMeshComponent* GetMesh() const { return Airframe; };

	UFUNCTION(BlueprintCallable)
	const FAircraftAnimationValues& GetAircraftAnimationValues();

	USkeletalMeshComponent* GetAirframe() const { return Airframe; };

	UBulletStats* GetBulletStats() const { return BulletStats; };

	UAircraftStats* GetAirStats() const { return AirStats; };

	URadarComponent* GetRadarComp() const { return RadarComponent; };

	UWeaponSystemComponent* GetWeaponComp() const { return WeaponComponent; };

	UFlightComponent* GetFlightComp() const { return FlightComponent; };

	UFUNCTION(BlueprintCallable)
	float GetGForce();

	UFUNCTION(BlueprintCallable)
	float GetMaxWeaponCount();

	UFUNCTION(BlueprintCallable)
	float GetCurrentWeaponCount();

	UBoxComponent* GetLandingGearCollision() const { return LandingGearCollision; };

	EThrottleStage GetThrottleStage() const;

	void ApplySpeed(const float Speed, const float D);

	void Crash();

	virtual void RegisterIncomingMissile_Implementation(ABaseMissile* Missile) override {
		IncomingMissiles.AddUnique(Missile);
	};

	virtual void UnregisterIncomingMissile_Implementation(ABaseMissile* Missile) override {
		IncomingMissiles.RemoveSingle(Missile);
	};

	virtual void OnCountermeasureDeployed_Implementation() override;

	bool IsLanded();

	UFUNCTION(BlueprintCallable)
	float GetSpeed();

protected:

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UAircraftStats> AirStats = nullptr;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UBulletStats> BulletStats = nullptr;

	UPROPERTY()
	TObjectPtr<UFlightComponent> FlightComponent = nullptr;

	UPROPERTY()
	TObjectPtr<UWeaponSystemComponent> WeaponComponent = nullptr;

	UPROPERTY()
	TObjectPtr<URadarComponent> RadarComponent = nullptr;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UAircraftVisualComponent> VisualCompClass = nullptr;

	UPROPERTY()
	TObjectPtr<UAircraftVisualComponent> VisualComp = nullptr;

	UPROPERTY()
	TObjectPtr<USpecialSystemComponent> SpecialComp = nullptr;

	UPROPERTY()
	FAircraftAnimationValues DefaultAnimVal{};

	UPROPERTY()
	TArray<TWeakObjectPtr<ABaseMissile>> IncomingMissiles{};

	bool bLocked = false;

	UPROPERTY(EditAnywhere)
	int32 NumOfAfterburners = 0;

	UPROPERTY(EditAnywhere)
	int32 NumOfVortices = 0;

	UPROPERTY(EditAnywhere)
	int32 NumOfMainWingVapors = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "VFX")
	TArray<UStaticMeshComponent*> AllMainWingVapors{};

	UPROPERTY()
	TArray<UNiagaraComponent*> AllAfterburners;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UNiagaraSystem> WingVortexSystem = nullptr;

	UPROPERTY()
	TArray<UNiagaraComponent*> AllVortices{};

	float AddedGearHeight = 0.f;

	UPROPERTY(EditAnywhere)
	float MaxSinkRate = 100.f;

	FVector OriginalCollOffset = FVector::ZeroVector;

	FVector OriginalExtent = FVector::ZeroVector;

	UPROPERTY()
	TObjectPtr<AActor> Tracking = nullptr;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USkeletalMeshComponent> Airframe = nullptr;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UBoxComponent> BodyCollision = nullptr;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UBoxComponent> LeftWingCollision = nullptr;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UBoxComponent> RightWingCollision = nullptr;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UBoxComponent> RudderCollision = nullptr;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UBoxComponent> LandingGearCollision = nullptr;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UNiagaraSystem> AfterburnerSystem = nullptr;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMeshComponent> LandingGear = nullptr;

private:

	UFUNCTION()
	void HandleAfterburnerFX(bool isActive);

	UFUNCTION()
	void HandleVortexFX(bool isActive);

	bool bIsVisible = true;

	UPROPERTY(EditAnywhere)
	float DropSpeed = 0.f;

	UPROPERTY(EditAnywhere)
	float StallSpeed = 0.f;

	bool bLanded = false;
};
