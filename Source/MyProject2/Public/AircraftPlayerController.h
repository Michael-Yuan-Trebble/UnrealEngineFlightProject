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

class APlayerAircraft;
class ASu25Pawn;
class UInputMappingContext;
class UEnhancedInputComponent;
class UMenuManagerComponent;
class UFlightComponent;
class URadarComponent;
class UWeaponSystemComponent;
class UCameraManagerComponent;

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

	TArray<EMenuState> MenuHistory;

	UMenuManagerComponent* MenuManager;

	UFlightComponent* FlightComp;

	UWeaponSystemComponent* WeaponComp;

	URadarComponent* RadarComp;

	UCameraManagerComponent* ManagerComp;

	TSubclassOf<ABaseWeapon> CurrentWeaponClass;

	void ManageMenuSetting(EMenuState NewState);

	//UVariables

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool isFlying = false;

	FTimerHandle RepeatTimerHandle;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	APlayerAircraft* Controlled;

	TArray<FDetectedAircraftInfo> Detected;

	AActor* Selected;

	int SelectedIndex = -1;

	void SetControlMode(EControlMode NewMode);

private:

	void BindAircraftInputs(UEnhancedInputComponent* EnhancedInputComp);
	void BindMenuInputs(UEnhancedInputComponent* EnhancedInputComp);

	//Movment, will move most calculations to the pawn
	void Thrust(const FInputActionValue& Value);
	void Roll(const FInputActionValue& Value);
	void Pitch(const FInputActionValue& Value);
	void Rudder(const FInputActionValue& Value);

	//Weapons
	void Weapons();
	void Special();
	void ShootStart();
	void ShootEnd();
	void Bullets();
	void NextWeapon();
	void PreviousWeapon();

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

	//Variables

	bool fire = false;

	float thrustPercentage = 0.5f;

	int CurrentWeaponIndex = 0;

	bool isThrust = false;
};
