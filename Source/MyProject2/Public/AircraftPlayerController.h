// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "Weapons/Missiles/BaseIRMissile.h"
#include "GameFramework/SpringArmComponent.h"
#include "Structs and Data/MenuState.h"
#include "Structs and Data/FDetectedAircraftInfo.h"
#include "Gamemodes/CurrentPlayerState.h"
#include "Structs and Data/ControlModeTypes.h"
#include "AircraftPlayerController.generated.h"

class ABaseAircraft;
class ASu25Pawn;
class UInputMappingContext;
class UEnhancedInputComponent;
class UMenuManagerComponent;

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
	UInputMappingContext* Mapping;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu Input")
	UInputMappingContext* MenuInputMapping;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu Input")
	UInputAction* Up;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu Input")
	UInputAction* Down;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu Input")
	UInputAction* IA_Back;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* Throttle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* IA_Roll;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* IA_Pitch;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* IA_Rudder;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* IA_Special;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* IA_Shoot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* IA_Weapons;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* IA_LookX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* IA_LookY;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* IA_Focus;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* IA_Switch;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* IA_Zoom;

	int32 AircraftMappingPriority = 0;
	int32 MenuMappingPriority = 1;

	EControlMode CurrentMode = EControlMode::Null;

	UPROPERTY()
	EMenuState CurrentMenuState;

	TArray<EMenuState> MenuHistory;

	UMenuManagerComponent* MenuManager;

	void ManageMenuSetting(EMenuState NewState);

	UUserWidget* CurrentWidget;

	//UVariables

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool isFlying = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float inputThrust;

	UPROPERTY(EditAnywhere)
	float inputRoll;

	UPROPERTY(EditAnywhere)
	float inputPitch;

	UPROPERTY(EditAnywhere)
	float inputYaw;

	float thrustNeeded;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UCameraComponent* CameraComp;

	FTimerHandle RepeatTimerHandle;

	ABaseAircraft* Controlled;

	TArray<FDetectedAircraftInfo> Detected;

	AActor* Selected;

	int SelectedIndex = -1;

	const TArray<FDetectedAircraftInfo>& GetDetectedTargets() const;

	void SetControlMode(EControlMode NewMode);

	void SetWidget(UUserWidget* Widget);

private:

	void BindAircraftInputs(UEnhancedInputComponent* EnhancedInputComp);
	void BindMenuInputs(UEnhancedInputComponent* EnhancedInputComp);

	//Movment, will move most calculations to the pawn
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

	void MenuBack();

	//U Stuff

	USkeletalMeshComponent* Airframe;

	USpringArmComponent* SpringArm;

	TArray<ABaseIRMissile*> WeaponList;

	//Variables

	bool fire = false;

	float LastActionTime = -2.f;

	float LastSpecialTime = -2.f;

	float thrustPercentage = 0.5f;

	float seePitch;

	float currentYaw;

	float lookX;

	float lookY;

	int CurrentWeaponIndex = 0;

	bool isThrust = false;
	float prevPitch;
	float prevYaw;
};
