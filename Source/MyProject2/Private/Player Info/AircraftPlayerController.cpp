// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("AH!"));
#include "Player Info/AircraftPlayerController.h"
#include "InputMappingContext.h"
#include "Units/Aircraft/MenuManagerComponent.h"
#include "Units/Aircraft/WeaponSystemComponent.h"
#include "Subsystem/AircraftRegistry.h"
#include "EnhancedInputSubsystems.h"
#include "UI/PlayerHUD.h"
#include "TimerManager.h"
#include "Weapons/Missiles/BaseMissile.h"
#include "Subsystem/MissileManagerSubsystem.h"
#include "Player Info/PlayerGameInstance.h"
#include "Units/Aircraft/Player/PlayerAircraft.h"

AAircraftPlayerController::AAircraftPlayerController() 
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	MenuManager = CreateDefaultSubobject<UMenuManagerComponent>(TEXT("MenuManager"));
}

void AAircraftPlayerController::BeginPlay() 
{
	Super::BeginPlay();
	if (IsValid(MenuManager)) MenuManager->InitializePC(this);
	if (UAircraftRegistry* Reg = UAircraftRegistry::Get(GetWorld()))
	{
		Reg->OnAnyUnitHit.AddUObject(this, &AAircraftPlayerController::OnUnitHit);
		Reg->OnAnyUnitDeath.AddUObject(this, &AAircraftPlayerController::OnUnitDestroyed);
	}
	GetWorld()->GetTimerManager().SetTimer(
		UpdateVFXHandle,
		this,
		&AAircraftPlayerController::UpdateLODs,
		ScanTime,
		true
	);
}

void AAircraftPlayerController::OnPossess(APawn* InPawn) 
{
	Super::OnPossess(InPawn);
	HUD = Cast<APlayerHUD>(GetHUD());
	Controlled = Cast<APlayerAircraft>(InPawn);
	if (IsValid(HUD)) HUD->Init(this);
	if (IsValid(Controlled))
	{
		Controlled->SetHUD(HUD);
		Controlled->OnMissileLaunchedAtSelf.AddDynamic(this, &AAircraftPlayerController::HandleMissileLaunchedAtPlayer);
		Controlled->OnLockedOnByEnemy.AddDynamic(this, &AAircraftPlayerController::HandleMissileLockedAtPlayer);
	}
}

void AAircraftPlayerController::SetComponents(UWeaponSystemComponent* InWeapon) 
{
	if (!IsValid(InWeapon)) return;
	InWeapon->OnWeaponHit.AddDynamic(this, &AAircraftPlayerController::HandleWeaponHit);
	InWeapon->OnHUDLockedOn.AddDynamic(this, &AAircraftPlayerController::HandleHUDLockedOn);
	InWeapon->OnWeaponCountUpdated.AddDynamic(this, &AAircraftPlayerController::HandleWeaponCount);
}

void AAircraftPlayerController::UpdateLODs()
{
	FVector CameraLoc = PlayerCameraManager->GetCameraLocation();
	// Missile LODs
	if (UMissileManagerSubsystem* Subsystem = GetGameInstance()->GetSubsystem<UMissileManagerSubsystem>())
	{
		for (TWeakObjectPtr<ABaseMissile> Missile : Subsystem->ActiveMissiles)
		{
			if (!Missile.IsValid()) continue;
			ABaseMissile* M = Missile.Get();
			M->ApplyVFXLOD(CameraLoc);
		}
	}

	// Airframe LODs
	if (UAircraftRegistry* Registry = UAircraftRegistry::Get(GetWorld())) 
	{
		for (TWeakObjectPtr<ABaseUnit> Unit : Registry->RegisteredUnits) 
		{
			if (!Unit.IsValid()) continue;
			ABaseUnit* U = Unit.Get();
			U->HandleLOD(CameraLoc);
		}
	}
}

void AAircraftPlayerController::SetFlightMode(const EFlightMode FlightMode) {

}

void AAircraftPlayerController::HandleWeaponHit(bool bHit)
{
	if (!bHit && IsValid(HUD)) HUD->HandleWeaponMiss();
}

void AAircraftPlayerController::OnUnitHit(AActor* Launcher) 
{
	if (IsValid(HUD) && Launcher == GetPawn()) HUD->UpdateTargetHit(false);
}

void AAircraftPlayerController::OnUnitDestroyed(AActor* Launcher) 
{
	if (IsValid(HUD) && Launcher == GetPawn()) HUD->UpdateTargetHit(true);
}

void AAircraftPlayerController::HandleHUDLockedOn(const float LockPercent)
{
	if (IsValid(HUD)) HUD->UpdateLocked(LockPercent);
}

void AAircraftPlayerController::HandleWeaponCount(FName WeaponName, int32 CurrentCount, int32 MaxCount) 
{
	if (IsValid(HUD)) HUD->OnWeaponChanged(WeaponName, CurrentCount, MaxCount);
}

void AAircraftPlayerController::HandleMissileLaunchedAtPlayer(ABaseMissile* Missile) 
{
	if (IsValid(HUD)) HUD->HandleMissileLaunchedAtSelf(Missile);
}

void AAircraftPlayerController::HandleMissileLockedAtPlayer() 
{
	if (IsValid(HUD)) HUD->HandleMissileLockedAtSelf();
}

// Setting controls

void AAircraftPlayerController::BindAircraftInputs(UEnhancedInputComponent* EnhancedInputComp) 
{
	if (!IsValid(EnhancedInputComp)) return;

	UFlightInputConfig* FlightConfig = FlightInputs.LoadSynchronous();
	if (!IsValid(FlightConfig)) return;

	// TODO: Maybe do a fallback to these if FlightInputs->IA stuff is null
	//FSoftObjectPath ThrottlePath(TEXT("/Game/Controls/Inputs/Throttle.Throttle"));
	//TSoftObjectPtr<UInputAction> SoftThrottle(ThrottlePath);
	//FlightInputs->IA_Throttle = SoftThrottle.LoadSynchronous();
	if (IsValid(FlightConfig->IA_Throttle))
	{
		EnhancedInputComp->BindAction(FlightInputs->IA_Throttle, ETriggerEvent::Triggered, this, &AAircraftPlayerController::Thrust);
		EnhancedInputComp->BindAction(FlightInputs->IA_Throttle, ETriggerEvent::Completed, this, &AAircraftPlayerController::Thrust);
	}
	if (IsValid(FlightConfig->IA_Roll))
	{
		EnhancedInputComp->BindAction(FlightInputs->IA_Roll, ETriggerEvent::Triggered, this, &AAircraftPlayerController::Roll);
		EnhancedInputComp->BindAction(FlightInputs->IA_Roll, ETriggerEvent::Completed, this, &AAircraftPlayerController::Roll);
	}
	if (IsValid(FlightConfig->IA_Pitch))
	{
		EnhancedInputComp->BindAction(FlightInputs->IA_Pitch, ETriggerEvent::Triggered, this, &AAircraftPlayerController::Pitch);
		EnhancedInputComp->BindAction(FlightInputs->IA_Pitch, ETriggerEvent::Completed, this, &AAircraftPlayerController::Pitch);
	}
	if (IsValid(FlightConfig->IA_Yaw))
	{
		EnhancedInputComp->BindAction(FlightInputs->IA_Yaw, ETriggerEvent::Triggered, this, &AAircraftPlayerController::Rudder);
		EnhancedInputComp->BindAction(FlightInputs->IA_Yaw, ETriggerEvent::Completed, this, &AAircraftPlayerController::Rudder);
	}
	if (IsValid(FlightConfig->IA_LookX))
	{
		EnhancedInputComp->BindAction(FlightInputs->IA_LookX, ETriggerEvent::Triggered, this, &AAircraftPlayerController::LookHor);
		EnhancedInputComp->BindAction(FlightInputs->IA_LookX, ETriggerEvent::Completed, this, &AAircraftPlayerController::LookHor);
	}
	if (IsValid(FlightConfig->IA_LookY))
	{
		EnhancedInputComp->BindAction(FlightInputs->IA_LookY, ETriggerEvent::Triggered, this, &AAircraftPlayerController::LookVer);
		EnhancedInputComp->BindAction(FlightInputs->IA_LookY, ETriggerEvent::Completed, this, &AAircraftPlayerController::LookVer);
	}
	if (IsValid(FlightConfig->IA_Focus))
	{
		EnhancedInputComp->BindAction(FlightInputs->IA_Focus, ETriggerEvent::Started, this, &AAircraftPlayerController::Focus);
		EnhancedInputComp->BindAction(FlightInputs->IA_Focus, ETriggerEvent::Ongoing, this, &AAircraftPlayerController::Focus);
		EnhancedInputComp->BindAction(FlightInputs->IA_Focus, ETriggerEvent::Completed, this, &AAircraftPlayerController::FocusStop);
	}
	if (IsValid(FlightConfig->IA_Switch))
	{
		EnhancedInputComp->BindAction(FlightInputs->IA_Switch, ETriggerEvent::Started, this, &AAircraftPlayerController::Switch);
	}
	if (IsValid(FlightConfig->IA_Zoom))
	{
		EnhancedInputComp->BindAction(FlightInputs->IA_Zoom, ETriggerEvent::Started, this, &AAircraftPlayerController::MapZoom);
		EnhancedInputComp->BindAction(FlightInputs->IA_Zoom, ETriggerEvent::Ongoing, this, &AAircraftPlayerController::MapZoom);
		EnhancedInputComp->BindAction(FlightInputs->IA_Zoom, ETriggerEvent::Completed, this, &AAircraftPlayerController::StopMapZoom);
	}
	if (IsValid(FlightConfig->IA_TogglePerspective))
	{
		EnhancedInputComp->BindAction(FlightInputs->IA_TogglePerspective, ETriggerEvent::Started, this, &AAircraftPlayerController::TogglePerspective);
	}
}

void AAircraftPlayerController::BindWeaponInputs(UEnhancedInputComponent* EnhancedInputComp) 
{
	if (!IsValid(EnhancedInputComp)) return;
	
	UWeaponInputConfig* WeaponConfig = WeaponInputs.LoadSynchronous();
	if (!IsValid(WeaponConfig)) return;

	if (IsValid(WeaponConfig->IA_Shoot))
	{
		EnhancedInputComp->BindAction(WeaponInputs->IA_Shoot, ETriggerEvent::Started, this, &AAircraftPlayerController::ShootStart);
		EnhancedInputComp->BindAction(WeaponInputs->IA_Shoot, ETriggerEvent::Completed, this, &AAircraftPlayerController::ShootEnd);
	}
	if (IsValid(WeaponConfig->IA_Weapons))
	{
		EnhancedInputComp->BindAction(WeaponInputs->IA_Weapons, ETriggerEvent::Started, this, &AAircraftPlayerController::Weapons);
	}
	if (IsValid(WeaponConfig->IA_NextWeapon))
	{
		EnhancedInputComp->BindAction(WeaponInputs->IA_NextWeapon, ETriggerEvent::Started, this, &AAircraftPlayerController::NextWeapon);
	}
	if (IsValid(WeaponConfig->IA_PrevWeapon))
	{
		EnhancedInputComp->BindAction(WeaponInputs->IA_PrevWeapon, ETriggerEvent::Started, this, &AAircraftPlayerController::PreviousWeapon);
	}
	if (IsValid(WeaponConfig->IA_Special))
	{
		EnhancedInputComp->BindAction(WeaponInputs->IA_Special, ETriggerEvent::Started, this, &AAircraftPlayerController::Special);
	}
}

void AAircraftPlayerController::BindMenuInputs(UEnhancedInputComponent* EnhancedInputComp) 
{
	if (!IsValid(EnhancedInputComp)) return;

	UMenuInputConfig* MenuConfig = MenuInputs.LoadSynchronous();
	if (!IsValid(MenuConfig)) return;

	if (IsValid(MenuConfig->IA_Up))
	{
		//EnhancedInputComp->BindAction(MenuInputs->IA_Up, ETriggerEvent::Triggered, this, &AAircraftPlayerController::Thrust);
		//EnhancedInputComp->BindAction(MenuInputs->IA_Up, ETriggerEvent::Completed, this, &AAircraftPlayerController::Thrust);
	}
	if (IsValid(MenuConfig->IA_Down))
	{
		//EnhancedInputComp->BindAction(MenuInputs->IA_Down, ETriggerEvent::Triggered, this, &AAircraftPlayerController::Thrust);
		//EnhancedInputComp->BindAction(MenuInputs->IA_Down, ETriggerEvent::Completed, this, &AAircraftPlayerController::Thrust);
	}
	if (IsValid(MenuConfig->IA_Back))
	{
		EnhancedInputComp->BindAction(MenuInputs->IA_Back, ETriggerEvent::Completed, this, &AAircraftPlayerController::MenuBack);
	}
}

void AAircraftPlayerController::SetupInputComponent() 
{
	Super::SetupInputComponent();
	if (UEnhancedInputComponent* EnhancedInputComp = Cast<UEnhancedInputComponent>(InputComponent)) 
	{
		BindAircraftInputs(EnhancedInputComp);
		BindWeaponInputs(EnhancedInputComp);
		BindMenuInputs(EnhancedInputComp);
	}
}

void AAircraftPlayerController::TogglePauseMenu() 
{
	
}

// TODO: Change between two control modes mid game based on options and active gameplay

void AAircraftPlayerController::SetControlMode(const EControlMode NewMode) 
{
	if (CurrentMode == NewMode || !GetLocalPlayer()) return;
	CurrentMode = NewMode;

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (!IsValid(Subsystem)) return;

	UEnhancedInputComponent* EnhancedInputComp = Cast<UEnhancedInputComponent>(InputComponent);
	if (!IsValid(EnhancedInputComp)) return;

	UInputMappingContext* MenuInputConfig = MenuInputMapping.LoadSynchronous();
	UInputMappingContext* MappingConfig = Mapping.LoadSynchronous();

	if (!IsValid(MenuInputConfig) || !IsValid(MappingConfig)) return;

	Subsystem->ClearAllMappings();
	switch (NewMode) {
		case EControlMode::Menu:
			Subsystem->AddMappingContext(MenuInputConfig, MenuMappingPriority);
			SetInputMode(FInputModeUIOnly());
			bShowMouseCursor = true;
			break;
		case EControlMode::Aircraft:
			Subsystem->AddMappingContext(MappingConfig, AircraftMappingPriority);
			SetInputMode(FInputModeGameOnly());
			bShowMouseCursor = false;
			break;
		default:
			break;
	}
}

// TODO: Currently passing Menu States, will delegate this to purely menumanager

void AAircraftPlayerController::MenuBack() 
{
	if (IsValid(MenuManager) && MenuHistory.Num() > 0) MenuManager->GoBack(MenuHistory.Pop());
}

void AAircraftPlayerController::ManageMenuSetting(const EMenuState NewMenu) 
{
	if (!IsValid(MenuManager)) return;
	MenuHistory.Push(NewMenu);
	MenuManager->GetWidgetClassForState(NewMenu);
}

//Neutral = 50% Throttle, increase/decrease accordingly
void AAircraftPlayerController::Thrust(const FInputActionValue& Value)
{
	if (!bMovementEnabled) return;
	float thrustNeeded = FMath::Clamp(thrustPercentage + (Value.Get<float>() / 2.f), 0, 1.f);
	thrustPercentage = FMath::FInterpTo(thrustPercentage, thrustNeeded, GetWorld()->GetDeltaSeconds(), 2.f);
	bThrust = Value.Get<float>() != 0;
} 

//Movement Functions

void AAircraftPlayerController::Roll(const FInputActionValue& Value) 
{
	if (!IsValid(Controlled)) return;
	if (!bMovementEnabled) {
		Controlled->SetRoll(0.f);
		return;
	}
	Controlled->SetRoll(Value.Get<float>());
}

void AAircraftPlayerController::Pitch(const FInputActionValue& Value)
{
	if (!IsValid(Controlled)) return;
	if (!bMovementEnabled) {
		Controlled->SetPitch(0.f);
		return;
	}
	Controlled->SetPitch(Value.Get<float>());
}

void AAircraftPlayerController::Rudder(const FInputActionValue& Value) 
{
	if (!IsValid(Controlled)) return;
	if (!bMovementEnabled) {
		Controlled->SetRudder(0.f);
		return;
	}
	Controlled->SetRudder(Value.Get<float>());
}

//Specials

void AAircraftPlayerController::Weapons()
{
	if (!bWeaponEnabled) return;
	if (IsValid(Controlled)) Controlled->FireWeaponSelected();
}

void AAircraftPlayerController::NextWeapon() 
{
	if (!bWeaponEnabled) return;
	if (IsValid(Controlled)) WeaponIndex = Controlled->AdvanceWeapon(WeaponIndex, true);
}

void AAircraftPlayerController::PreviousWeapon() 
{
	if (!bWeaponEnabled) return;
	if (IsValid(Controlled)) WeaponIndex = Controlled->AdvanceWeapon(WeaponIndex, false);
}

void AAircraftPlayerController::Special() 
{
	if (!bWeaponEnabled) return;
	if (IsValid(Controlled)) Controlled->ActivateSpecial();
}

//Gun

void AAircraftPlayerController::ShootStart() 
{
	if (!bWeaponEnabled) ShootEnd();
	if (bFire) return;
	bFire = true;
	if (IsValid(Controlled)) Controlled->StartBullets();
}

void AAircraftPlayerController::ShootEnd() 
{
	if (!bFire) return;
	bFire = false;
	if (IsValid(Controlled)) Controlled->EndBullets();
}

void AAircraftPlayerController::Bullets() 
{
	if (!bWeaponEnabled) ShootEnd();
	if (IsValid(Controlled)) Controlled->FireBullets();
}

//Camera Movement

void AAircraftPlayerController::Switch() 
{
	if (!bWeaponEnabled) return;
	if (IsValid(Controlled)) Controlled->CycleTarget();
}

void AAircraftPlayerController::TogglePerspective() 
{
	if (!bCameraEnabled) return;
	if (IsValid(Controlled)) Controlled->SwitchCameras();
}

void AAircraftPlayerController::Focus()
{

}

void AAircraftPlayerController::FocusStop()
{

}

void AAircraftPlayerController::LookHor(const FInputActionValue& ValueX) 
{
	if (!bCameraEnabled) return;
	if (IsValid(Controlled)) Controlled->HandleHorizontal(ValueX.Get<float>());
}

void AAircraftPlayerController::LookVer(const FInputActionValue& ValueY) 
{
	if (!bCameraEnabled) return;
	if (IsValid(Controlled)) Controlled->HandleVertical(ValueY.Get<float>());
}

//Map

void AAircraftPlayerController::MapZoom()
{

}

void AAircraftPlayerController::StopMapZoom()
{

}

//Might Make two states so Tick doesn't have to do as much, one grounded and one not
void AAircraftPlayerController::Tick(float DeltaSeconds) 
{
	Super::Tick(DeltaSeconds);
	if (CurrentMode == EControlMode::Menu) return;
	if (!bThrust) thrustPercentage = FMath::FInterpTo(thrustPercentage, MiddleThrust, DeltaSeconds, 2.f);
	if (IsValid(Controlled)) Controlled->SetThrust(thrustPercentage);
}