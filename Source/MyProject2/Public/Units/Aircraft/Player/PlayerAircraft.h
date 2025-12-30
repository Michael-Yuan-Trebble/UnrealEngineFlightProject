// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Units/Aircraft/BaseAircraft.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "PlayerAircraft.generated.h"

class AAircraftPlayerController;
class UCameraManagerComponent;
class UAircraftAudioComponent;
class ABaseWeapon;
class APlayerHUD;

UCLASS()
class MYPROJECT2_API APlayerAircraft : public ABaseAircraft
{
	GENERATED_BODY()
public:
	APlayerAircraft();

	virtual void PossessedBy(AController* Controller) override;

	UPROPERTY()
	AAircraftPlayerController* Controlled;

	UPROPERTY()
	UCameraManagerComponent* ManagerComp;

	UPROPERTY(EditAnywhere)
	float CameraSens = 1.f;

	UPROPERTY(EditAnywhere)
	UAircraftAudioComponent* AudioComp;

	UPROPERTY()
	UAudioComponent* PersonalAircraftAudio;

	UPROPERTY()
	UAudioComponent* GunAudio;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USpringArmComponent* FirstPersonSpringArm;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USpringArmComponent* ThirdPersonSpringArm;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCameraComponent* ThirdPersonCamera;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCameraComponent* FirstPersonCamera;

	USpringArmComponent* GetFirstPersonSpringArm() const { return FirstPersonSpringArm; };

	USpringArmComponent* GetThirdPersonSpringArm() const { return ThirdPersonSpringArm; };

	UCameraComponent* GetThirdPersonCamera() const { return ThirdPersonCamera; };

	UCameraComponent* GetFirstPersonCamera() const{ return FirstPersonCamera; };

	void SetFirstPersonCamera(bool bActive);

	void SetThirdPersonCamera(bool bActive);

	float ReturnSpringArmLength() const { return springArmLength; };

	UAudioComponent* GetAircraftAudio() const { return PersonalAircraftAudio; };

	UAudioComponent* GetGunAudio() const { return GunAudio; };

	float GetCameraLocation() const;

	void GunSoundEffect(bool bShoot);

	void WeaponComponentOnUnitDeath();

	FTimerHandle RepeatTimerHandle;

// Controller Functions
public:

	// Weapon Component
	virtual void FireBullets() override;
	
	virtual void StartBullets() override;
	virtual void EndBullets() override;

	void SelectWeapon(float index);
	int32 AdvanceWeapon(int32 index, bool bForward);

	// Radar Component
	void CycleTarget();

	UPROPERTY(EditAnywhere)
	float RollLagStrength = 0.25f;

	UPROPERTY(EditAnywhere)
	float RollLagSpeed = 4.f;

	UPROPERTY(EditAnywhere)
	float Interp = 1.f;

	void SetInterp();

	float GetRoll();

	// Camera Manager
	void SetHUD(APlayerHUD* InHUD);
	void SwitchCameras();
	void HandleVertical(float Vertical);
	void HandleHorizontal(float Horizontal);
	void SetSensitivity(float Sens);
	void SetRollStrength(float S);
	void SetRollSpeed(float S);

	float GetThrottle();

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

private:
	UFUNCTION()
	void HandleHit();

	float OriginalFirstPersonSpringArmLength = 0.f;
	float OriginalThirdPersonSpringArmLength = 0.f;
};
