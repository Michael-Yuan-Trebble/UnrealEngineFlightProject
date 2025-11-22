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

UCLASS()
class MYPROJECT2_API ABaseAircraft : public ABaseUnit
{
	GENERATED_BODY()

public:
	ABaseAircraft();

	UPROPERTY(EditAnywhere)
	UAircraftStats* AirStats;

	UPROPERTY(EditAnywhere)
	UBulletStats* BulletStats;

	UPROPERTY(BlueprintReadWrite)
	UFlightComponent* FlightComponent;

	UPROPERTY(BlueprintReadWrite)
	UWeaponSystemComponent* WeaponComponent;

	URadarComponent* RadarComponent;

	float springArmLength;

	float TakeoffSpeed;

	float InputPitchValue;

	float InputRollValue;

	float InputYawValue;

	float InputThrust;

	float LockTime;

	bool bLocked;

	UPROPERTY(EditAnywhere)
	int32 NumOfAfterburners;

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

	//Return Functions for UObjects

	virtual USkeletalMeshComponent* GetMesh() const { return Airframe; };

	//Return Functions for Vars

	virtual float ReturnTakeoffSpeed() const { return TakeoffSpeed; };

	virtual void PossessedBy(AController* Controller) override;

protected:

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	void ActivateAfterburnerFX();
	void DeactivateAfterburnerFX();

private:

	UFUNCTION()
	void HandleAfterburnerFX(bool isActive);
};
