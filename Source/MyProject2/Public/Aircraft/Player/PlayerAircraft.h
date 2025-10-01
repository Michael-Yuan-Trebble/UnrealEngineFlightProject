// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Aircraft/BaseAircraft.h"
#include "PlayerAircraft.generated.h"

class AAircraftPlayerController;
class UFlightComponent;
class UWeaponSystemComponent;
class URadarComponent;

UCLASS()
class MYPROJECT2_API APlayerAircraft : public ABaseAircraft
{
	GENERATED_BODY()
public:
	APlayerAircraft();

	USpringArmComponent* GetSpringArm() const;

	UCameraComponent* GetCamera() const;

	virtual void PossessedBy(AController* Controller) override;

	AAircraftPlayerController* Controlled;

	UFlightComponent* FlightComponent;

	UWeaponSystemComponent* WeaponComponent;

	URadarComponent* RadarComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USpringArmComponent* SpringArm;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCameraComponent* Camera;

	void SetWeapons(TMap<FName,TSubclassOf<ABaseWeapon>> In);

	float ReturnSpringArmLength() const;

	void SetStats(UAircraftStats* InStats);

	UFlightComponent* GetFlightComp();

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;
};
