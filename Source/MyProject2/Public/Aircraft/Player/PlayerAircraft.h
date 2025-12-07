// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Aircraft/BaseAircraft.h"
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

	AAircraftPlayerController* Controlled;

	UCameraManagerComponent* ManagerComp;

	UPROPERTY(EditAnywhere)
	UAircraftAudioComponent* AudioComp;

	UAudioComponent* PersonalAircraftAudio;

	UAudioComponent* GunAudio;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USpringArmComponent* SpringArm;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCameraComponent* ThirdPersonCamera;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCameraComponent* FirstPersonCamera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USpringArmComponent* FirstPersonSpringArm;

	USpringArmComponent* GetSpringArm() const { return SpringArm; };

	UCameraComponent* GetThirdPersonCamera() const { return ThirdPersonCamera; };

	UCameraComponent* GetFirstPersonCamera() const{ return FirstPersonCamera; };

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

	// Camera Manager
	void SetHUD(APlayerHUD* InHUD);
	void SwitchCameras();
	void HandleVertical(float Vertical);
	void HandleHorizontal(float Horizontal);

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

private:
	UFUNCTION()
	void HandleHit();
};
