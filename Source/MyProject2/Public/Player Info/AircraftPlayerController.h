// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Enums/MenuState.h"
#include "Enums/ControlModeTypes.h"
#include "Enums/FlightMode.h"
#include "Structs and Data/FlightInputConfig.h"
#include "Structs and Data/WeaponInputConfig.h"
#include "Structs and Data/MenuInputConfig.h"
#include "AircraftPlayerController.generated.h"

class APlayerAircraft;
class UInputMappingContext;
class UEnhancedInputComponent;
class UMenuManagerComponent;
class APlayerHUD;
class ABaseMissile;

UCLASS()
class MYPROJECT2_API AAircraftPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	//UInputs

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TSoftObjectPtr<UInputMappingContext> Mapping = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu Input")
	TSoftObjectPtr<UInputMappingContext> MenuInputMapping = nullptr;

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UFlightInputConfig> FlightInputs = nullptr;

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UWeaponInputConfig> WeaponInputs = nullptr;

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UMenuInputConfig> MenuInputs = nullptr;

	// Functions

	void ManageMenuSetting(const EMenuState NewState);
	void SetComponents(class UWeaponSystemComponent* InWeapon);
	void SetControlMode(const EControlMode NewMode);
	void SetFlightMode(const EFlightMode FlightMode);

	void DeactivateMovement() { bMovementEnabled = false; };
	void ActivateMovement() { bMovementEnabled = true; };
	void DeactivateCamera() { bCameraEnabled = false; };
	void ActivateCamera() { bCameraEnabled = true; };
	void DeactivateWeapon() { bWeaponEnabled = false; };
	void ActivateWeapon() { bWeaponEnabled = true; };

	UMenuManagerComponent* GetMenuManager() const { return MenuManager; };
	TArray<EMenuState> GetMenuHistory() const { return MenuHistory; };

private:

	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void OnPossess(APawn* InPawn) override;

	AAircraftPlayerController();

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

	bool bFire = false;
	bool bThrust = false;

	float thrustPercentage = MiddleThrust;
	int32 WeaponIndex = 0;

	FTimerHandle UpdateVFXHandle{};

	int32 AircraftMappingPriority = 0;
	int32 MenuMappingPriority = 1;

	EControlMode CurrentMode = EControlMode::Null;
	TArray<EMenuState> MenuHistory{};

	UPROPERTY()
	TObjectPtr<APlayerAircraft> Controlled = nullptr;

	UPROPERTY()
	TObjectPtr<UMenuManagerComponent> MenuManager = nullptr;

	UPROPERTY()
	TObjectPtr<APlayerHUD> HUD = nullptr;

	static constexpr float MiddleThrust = 0.5f;
	static constexpr float ScanTime = 0.15f;

	bool bMovementEnabled = true;
	bool bCameraEnabled = true;
	bool bWeaponEnabled = true;
};
