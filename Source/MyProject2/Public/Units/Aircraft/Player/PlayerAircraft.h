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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USpringArmComponent* FirstPersonSpringArm = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USpringArmComponent* ThirdPersonSpringArm = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCameraComponent* ThirdPersonCamera = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCameraComponent* FirstPersonCamera = nullptr;

	void GunSoundEffect(bool bShoot);

	void WeaponComponentOnUnitDeath();

	FTimerHandle RepeatTimerHandle;

public:

	USpringArmComponent* GetFirstPersonSpringArm() const { return FirstPersonSpringArm; };

	USpringArmComponent* GetThirdPersonSpringArm() const { return ThirdPersonSpringArm; };

	UCameraComponent* GetThirdPersonCamera() const { return ThirdPersonCamera; };

	UCameraComponent* GetFirstPersonCamera() const { return FirstPersonCamera; };

	void SetFirstPersonCamera(bool bActive);

	void SetThirdPersonCamera(bool bActive);

	UAudioComponent* GetAircraftAudio() const { return PersonalAircraftAudio; };

	UAudioComponent* GetGunAudio() const { return GunAudio; };

	float GetCameraLocation() const;

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

	void SetInterp();

	const FRotator& GetNextRotation();

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

	UPROPERTY(EditAnywhere)
	float CameraSens = 1.f;

	UPROPERTY(EditAnywhere)
	float RollLagStrength = 0.25f;

	UPROPERTY(EditAnywhere)
	float RollLagSpeed = 4.f;

	UPROPERTY(EditAnywhere)
	float Interp = 1.f;

	UPROPERTY()
	AAircraftPlayerController* Controlled = nullptr;

	UPROPERTY()
	UCameraManagerComponent* ManagerComp = nullptr;

	UPROPERTY(EditAnywhere)
	UAircraftAudioComponent* AudioComp = nullptr;

	UPROPERTY()
	UAudioComponent* PersonalAircraftAudio = nullptr;

	UPROPERTY()
	UAudioComponent* GunAudio = nullptr;

private:
	UFUNCTION()
	void HandleHit();

	float OriginalFirstPersonSpringArmLength = 0.f;
	float OriginalThirdPersonSpringArmLength = 0.f;
};
