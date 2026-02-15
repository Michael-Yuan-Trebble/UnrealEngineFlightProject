// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Enums/MenuState.h"
#include "Enums/ControlModeTypes.h"
#include "Enums/FlightMode.h"
#include "AircraftPlayerController.generated.h"

#define MIDDLETHRUST 0.5f

class APlayerAircraft;
class UInputMappingContext;
class UEnhancedInputComponent;
class UMenuManagerComponent;
class UWeaponSystemComponent;
class APlayerHUD;
class ABaseMissile;

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
	UInputAction* Up = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu Input")
	UInputAction* Down = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu Input")
	UInputAction* IA_Back = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* Throttle = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* IA_Roll = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* IA_Pitch = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* IA_Rudder = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* IA_Special = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* IA_Shoot = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* IA_Weapons = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* IA_LookX = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* IA_LookY = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* IA_Focus = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* IA_Switch = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* IA_Zoom = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* IA_NextWeapon = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* IA_PrevWeapon = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* IA_Menu = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* IA_TogglePerspective = nullptr;

	// Functions

	void ManageMenuSetting(const EMenuState NewState);
	void SetComponents(UWeaponSystemComponent* InWeapon);
	void SetControlMode(const EControlMode NewMode);
	void SetFlightMode(const EFlightMode FlightMode);

	void DeactivateMovement() { bMovementEnabled = false; };
	void ActivateMovement() { bMovementEnabled = true; };
	void DeactivateCamera() { bCameraEnabled = false; };
	void ActivateCamera() { bCameraEnabled = true; };
	void DeactivateWeapon() { bWeaponEnabled = false; };
	void ActivateWeapon() { bWeaponEnabled = true; };

	// Vars

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool isFlying = false;

	int SelectedIndex = -1;
	bool bIsPaused = false;

	int32 AircraftMappingPriority = 0;
	int32 MenuMappingPriority = 1;

	EControlMode CurrentMode = EControlMode::Null;
	TArray<EMenuState> MenuHistory;

	UPROPERTY()
	APlayerAircraft* Controlled = nullptr;

	UPROPERTY()
	UMenuManagerComponent* MenuManager = nullptr;

	UPROPERTY()
	UWeaponSystemComponent* WeaponComp = nullptr;

	UPROPERTY()
	APlayerHUD* HUD = nullptr;

private:

	void BindAircraftInputs(UEnhancedInputComponent* EnhancedInputComp);
	void BindMenuInputs(UEnhancedInputComponent* EnhancedInputComp);
	void BindWeaponInputs(UEnhancedInputComponent* EnhancedInputComp);

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
	void TogglePerspective();

	//Map
	void MapZoom();
	void StopMapZoom();

	void MenuBack();
	void TogglePauseMenu();

	UFUNCTION()
	void HandleWeaponHit(bool bHit);

	UFUNCTION()
	void HandleHUDLockedOn(const float LockPercent);

	UFUNCTION()
	void HandleWeaponCount(FName WeaponName, int32 CurrentCount, int32 MaxCount);

	UFUNCTION()
	void OnUnitHit(AActor* Launcher);

	UFUNCTION()
	void OnUnitDestroyed(AActor* Launcher);

	UFUNCTION()
	void HandleMissileLaunchedAtPlayer(ABaseMissile* Missile);

	UFUNCTION()
	void HandleMissileLockedAtPlayer();

	void UpdateLODs();

	//Variables

	bool fire = false;
	bool isThrust = false;

	float thrustPercentage = MiddleThrust;
	int32 WeaponIndex = 0;

	FTimerHandle UpdateVFXHandle;

	static constexpr float MiddleThrust = 0.5f;

	bool bMovementEnabled = true;
	bool bCameraEnabled = true;
	bool bWeaponEnabled = true;
};
