// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "BaseIRMissile.h"
#include "FDetectedAircraftInfo.h"
#include "CurrentPlayerState.h"
#include "LockBoxWidget.h"
#include "AircraftPlayerController.generated.h"

class ABaseAircraft;
class ASu25Pawn;
class UInputMappingContext;
class UEnhancedInputComponent;

UCLASS()
class MYPROJECT2_API AAircraftPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:

	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void OnPossess(APawn* InPawn) override;

	AAircraftPlayerController();

public:

	//UInputs

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	class UInputMappingContext* Mapping;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	class UInputAction* Throttle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	class UInputAction* IA_Roll;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	class UInputAction* IA_Pitch;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	class UInputAction* IA_Rudder;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	class UInputAction* IA_Special;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	class UInputAction* IA_Shoot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	class UInputAction* IA_Weapons;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	class UInputAction* IA_LookX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	class UInputAction* IA_LookY;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	class UInputAction* IA_Focus;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	class UInputAction* IA_Switch;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	class UInputAction* IA_Zoom;

	//UVariables

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool isFlying = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float currentPitch;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float currentRoll;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float currentRudder;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float currentSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector vectorLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float inputThrust;

	UPROPERTY(EditAnywhere)
	float inputRoll;

	UPROPERTY(EditAnywhere)
	float inputPitch;

	UPROPERTY(EditAnywhere)
	float inputYaw;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UCameraComponent* CameraComp;

	FTimerHandle RepeatTimerHandle;

	ABaseAircraft* Controlled;

	TArray<FDetectedAircraftInfo> Detected;

	AActor* Selected;

	bool GetTargetScreenPosition(AActor* Target, FVector2D& OutScreenPos) const;

	int SelectedIndex = -1;
	
	UPROPERTY()
	TMap<AActor*, ULockBoxWidget*> ActiveLockOnWidgets;

	TSubclassOf<ULockBoxWidget> LockBoxWidgetClasses;

private:

	//Movment, will move most calculations to the pawn
	void SpeedAdd(float ThrustPercentage, float prevSpeed);
	void Thrust(const FInputActionValue& Value);
	void Roll(const FInputActionValue& Value);
	void Pitch(const FInputActionValue& Value);
	void Rudder(const FInputActionValue& Value);

	void CycleTarget();
	void CycleToNextTarget();

	void ScanTargets();

	//Weapons
	void Weapons();
	void Special();
	void ShootStart();
	void ShootEnd();
	void Bullets();

	//Camera
	void LookHor(const FInputActionValue& ValueX);
	void LookVer(const FInputActionValue& ValueY);
	void Focus();
	void FocusStop();
	void Switch();

	//Map
	void MapZoom();
	void StopMapZoom();

	//U Stuff

	USkeletalMeshComponent* Airframe;

	class USpringArmComponent* SpringArm;

	ACurrentPlayerState* PS;

	TArray<ABaseIRMissile*> WeaponList;

	//Variables

	float planeAcceleration;

	float turnRate;

	float roll;

	float rudderRate;

	float maxSpeed;

	float drag = 0.f;

	float power;

	bool fire = false;

	float LastActionTime = -2.f;

	float LastSpecialTime = -2.f;

	float thrustPercentage = 0.5f;

	float springArmLengthOriginal;

	float seePitch;

	float currentYaw;

	float lookX;

	float lookY;

	float CameraLagSpeed = 10.f;

	int CurrentWeaponIndex = 0;

	bool isRoll = false;
	bool isPitch = false;
	bool isYaw = false;
	bool isThrust = false;
	float targetSpringArm;
	float prevPitch;
	float prevYaw;
	float thrustNeeded;
};
