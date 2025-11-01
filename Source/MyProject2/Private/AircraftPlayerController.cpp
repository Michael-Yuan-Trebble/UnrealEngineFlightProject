// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("AH!"));
#include "AircraftPlayerController.h"
#include "InputMappingContext.h"
#include "Aircraft/FlightComponent.h"
#include "Math/UnrealMathUtility.h"
#include "GameFramework/SpectatorPawn.h"
#include "Aircraft/MenuManagerComponent.h"
#include "Aircraft/WeaponSystemComponent.h"
#include "Aircraft/Player/CameraManagerComponent.h"
#include "Aircraft/RadarComponent.h"
#include "UI/PlayerHUD.h"
#include "Gamemodes/CurrentPlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Aircraft/Player/PlayerAircraft.h"

AAircraftPlayerController::AAircraftPlayerController() 
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	MenuManager = CreateDefaultSubobject<UMenuManagerComponent>(TEXT("MenuManager"));

	static ConstructorHelpers::FObjectFinder<UInputMappingContext> AircraftMapFinder(TEXT("/Game/Controls/Mapping.Mapping"));
	if (AircraftMapFinder.Succeeded()) 
	{
		Mapping = AircraftMapFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputMappingContext> MenuMapFinder(TEXT("/Game/Controls/MenuInputMapping.MenuInputMapping"));
	if (MenuMapFinder.Succeeded()) 
	{
		MenuInputMapping = MenuMapFinder.Object;
	}
}

void AAircraftPlayerController::BeginPlay() 
{
	Super::BeginPlay();
	ACurrentPlayerState* PS = Cast<ACurrentPlayerState>(this->PlayerState);
	MenuManager->InitializePC(this, PS);
	HUD = Cast<APlayerHUD>(GetHUD());
	if (HUD) HUD->PC = this;
}

void AAircraftPlayerController::OnPossess(APawn* InPawn) 
{
	Super::OnPossess(InPawn);
	if (!HUD || !InPawn) return;
	HUD->Init();
}

// Setting controls

void AAircraftPlayerController::BindAircraftInputs(UEnhancedInputComponent* EnhancedInputComp) 
{
	if (!EnhancedInputComp) return;
	FSoftObjectPath ThrottlePath(TEXT("/Game/Controls/Inputs/Throttle.Throttle"));
	TSoftObjectPtr<UInputAction> SoftThrottle(ThrottlePath);
	Throttle = SoftThrottle.LoadSynchronous();
	if (Throttle)
	{
		EnhancedInputComp->BindAction(Throttle, ETriggerEvent::Triggered, this, &AAircraftPlayerController::Thrust);
		EnhancedInputComp->BindAction(Throttle, ETriggerEvent::Completed, this, &AAircraftPlayerController::Thrust);
	}
	FSoftObjectPath RollPath(TEXT("/Game/Controls/Inputs/IA_Roll.IA_Roll"));
	TSoftObjectPtr<UInputAction> SoftRoll(RollPath);
	IA_Roll = SoftRoll.LoadSynchronous();
	if (IA_Roll)
	{
		EnhancedInputComp->BindAction(IA_Roll, ETriggerEvent::Triggered, this, &AAircraftPlayerController::Roll);
		EnhancedInputComp->BindAction(IA_Roll, ETriggerEvent::Completed, this, &AAircraftPlayerController::Roll);
	}
	FSoftObjectPath PitchPath(TEXT("/Game/Controls/Inputs/IA_Pitch.IA_Pitch"));
	TSoftObjectPtr<UInputAction> SoftPitch(PitchPath);
	IA_Pitch = SoftPitch.LoadSynchronous();
	if (IA_Pitch)
	{
		EnhancedInputComp->BindAction(IA_Pitch, ETriggerEvent::Triggered, this, &AAircraftPlayerController::Pitch);
		EnhancedInputComp->BindAction(IA_Pitch, ETriggerEvent::Completed, this, &AAircraftPlayerController::Pitch);
	}
	FSoftObjectPath RudderPath(TEXT("/Game/Controls/Inputs/IA_Rudder.IA_Rudder"));
	TSoftObjectPtr<UInputAction> SoftRudder(RudderPath);
	IA_Rudder = SoftRudder.LoadSynchronous();
	if (IA_Rudder)
	{
		EnhancedInputComp->BindAction(IA_Rudder, ETriggerEvent::Triggered, this, &AAircraftPlayerController::Rudder);
		EnhancedInputComp->BindAction(IA_Rudder, ETriggerEvent::Completed, this, &AAircraftPlayerController::Rudder);
	}
	FSoftObjectPath SpecialPath(TEXT("/Game/Controls/Inputs/IA_Special.IA_Special"));
	TSoftObjectPtr<UInputAction> SoftSpecial(SpecialPath);
	IA_Special = SoftSpecial.LoadSynchronous();
	if (IA_Special)
	{
		EnhancedInputComp->BindAction(IA_Special, ETriggerEvent::Started, this, &AAircraftPlayerController::Special);
	}
	FSoftObjectPath LookXPath(TEXT("/Game/Controls/Inputs/IA_LookX.IA_LookX"));
	TSoftObjectPtr<UInputAction> SoftLookX(LookXPath);
	IA_LookX = SoftLookX.LoadSynchronous();
	if (IA_LookX)
	{
		EnhancedInputComp->BindAction(IA_LookX, ETriggerEvent::Triggered, this, &AAircraftPlayerController::LookHor);
		EnhancedInputComp->BindAction(IA_LookX, ETriggerEvent::Completed, this, &AAircraftPlayerController::LookHor);
	}
	FSoftObjectPath LookYPath(TEXT("/Game/Controls/Inputs/IA_LookY.IA_LookY"));
	TSoftObjectPtr<UInputAction> SoftLookY(LookYPath);
	IA_LookY = SoftLookY.LoadSynchronous();
	if (IA_LookY)
	{
		EnhancedInputComp->BindAction(IA_LookY, ETriggerEvent::Triggered, this, &AAircraftPlayerController::LookVer);
		EnhancedInputComp->BindAction(IA_LookY, ETriggerEvent::Completed, this, &AAircraftPlayerController::LookVer);
	}
	FSoftObjectPath FocusPath(TEXT("/Game/Controls/Inputs/IA_Focus.IA_Focus"));
	TSoftObjectPtr<UInputAction> SoftFocus(FocusPath);
	IA_Focus = SoftFocus.LoadSynchronous();
	if (IA_Focus)
	{
		EnhancedInputComp->BindAction(IA_Focus, ETriggerEvent::Started, this, &AAircraftPlayerController::Focus);
		EnhancedInputComp->BindAction(IA_Focus, ETriggerEvent::Ongoing, this, &AAircraftPlayerController::Focus);
		EnhancedInputComp->BindAction(IA_Focus, ETriggerEvent::Completed, this, &AAircraftPlayerController::FocusStop);
	}
	FSoftObjectPath SwitchPath(TEXT("/Game/Controls/Inputs/IA_Switch.IA_Switch"));
	TSoftObjectPtr<UInputAction> SoftSwitch(SwitchPath);
	IA_Switch = SoftSwitch.LoadSynchronous();
	if (IA_Switch)
	{
		EnhancedInputComp->BindAction(IA_Switch, ETriggerEvent::Started, this, &AAircraftPlayerController::Switch);
	}
	FSoftObjectPath ZoomPath(TEXT("/Game/Controls/Inputs/IA_Zoom.IA_Zoom"));
	TSoftObjectPtr<UInputAction> SoftZoom(ZoomPath);
	IA_Zoom = SoftZoom.LoadSynchronous();
	if (IA_Zoom)
	{
		EnhancedInputComp->BindAction(IA_Zoom, ETriggerEvent::Started, this, &AAircraftPlayerController::MapZoom);
		EnhancedInputComp->BindAction(IA_Zoom, ETriggerEvent::Ongoing, this, &AAircraftPlayerController::MapZoom);
		EnhancedInputComp->BindAction(IA_Zoom, ETriggerEvent::Completed, this, &AAircraftPlayerController::StopMapZoom);
	}
}

void AAircraftPlayerController::BindWeaponInputs(UEnhancedInputComponent* EnhancedInputComp) 
{
	if (!EnhancedInputComp) return;
	FSoftObjectPath ShootPath(TEXT("/Game/Controls/Inputs/IA_Shoot.IA_Shoot"));
	TSoftObjectPtr<UInputAction> SoftShoot(ShootPath);
	IA_Shoot = SoftShoot.LoadSynchronous();
	if (IA_Shoot)
	{
		EnhancedInputComp->BindAction(IA_Shoot, ETriggerEvent::Started, this, &AAircraftPlayerController::ShootStart);
		EnhancedInputComp->BindAction(IA_Shoot, ETriggerEvent::Completed, this, &AAircraftPlayerController::ShootEnd);
	}
	FSoftObjectPath WeaponsPath(TEXT("/Game/Controls/Inputs/IA_Weapons.IA_Weapons"));
	TSoftObjectPtr<UInputAction> SoftWeapons(WeaponsPath);
	IA_Weapons = SoftWeapons.LoadSynchronous();
	if (IA_Weapons)
	{
		EnhancedInputComp->BindAction(IA_Weapons, ETriggerEvent::Started, this, &AAircraftPlayerController::Weapons);
	}
	FSoftObjectPath NextWeaponPath(TEXT("/Game/Controls/Inputs/IA_NextWeapon.IA_NextWeapon"));
	TSoftObjectPtr<UInputAction> SoftNext(NextWeaponPath);
	IA_NextWeapon = SoftNext.LoadSynchronous();
	if (IA_NextWeapon) 
	{
		EnhancedInputComp->BindAction(IA_NextWeapon, ETriggerEvent::Started, this, &AAircraftPlayerController::NextWeapon);
	}
	FSoftObjectPath PrevWeaponPath(TEXT("/Game/Controls/Inputs/IA_PrevWeapon.IA_PrevWeapon"));
	TSoftObjectPtr<UInputAction> SoftPrev(PrevWeaponPath);
	IA_PrevWeapon = SoftPrev.LoadSynchronous();
	if (IA_PrevWeapon) 
	{
		EnhancedInputComp->BindAction(IA_PrevWeapon, ETriggerEvent::Started, this, &AAircraftPlayerController::PreviousWeapon);
	}
}

void AAircraftPlayerController::BindMenuInputs(UEnhancedInputComponent* EnhancedInputComp) 
{
	if (!EnhancedInputComp) return;
	FSoftObjectPath UpPath(TEXT("/Game/Controls/MenuInputs/Up.Up"));
	TSoftObjectPtr<UInputAction> SoftUp(UpPath);
	Up = SoftUp.LoadSynchronous();
	if (Up)
	{
		//EnhancedInputComp->BindAction(Up, ETriggerEvent::Triggered, this, &AAircraftPlayerController::Thrust);
		//EnhancedInputComp->BindAction(Up, ETriggerEvent::Completed, this, &AAircraftPlayerController::Thrust);
	}
	FSoftObjectPath DownPath(TEXT("/Game/Controls/MenuInputs/Down.Down"));
	TSoftObjectPtr<UInputAction> SoftDown(DownPath);
	Down = SoftDown.LoadSynchronous();
	if (Down)
	{
		//EnhancedInputComp->BindAction(Up, ETriggerEvent::Triggered, this, &AAircraftPlayerController::Thrust);
		//EnhancedInputComp->BindAction(Up, ETriggerEvent::Completed, this, &AAircraftPlayerController::Thrust);
	}
	FSoftObjectPath BackPath(TEXT("/Game/Controls/MenuInputs/Back.Back"));
	TSoftObjectPtr<UInputAction> SoftBack(BackPath);
	IA_Back = SoftBack.LoadSynchronous();
	if (IA_Back)
	{
		EnhancedInputComp->BindAction(IA_Back, ETriggerEvent::Completed, this, &AAircraftPlayerController::MenuBack);
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

void AAircraftPlayerController::TogglePauseMenu() {
	
}

// TODO: Change between two control modes mid game based on options and active gameplay

void AAircraftPlayerController::SetControlMode(EControlMode NewMode) 
{
	if (CurrentMode == NewMode || !GetLocalPlayer()) return;
	CurrentMode = NewMode;

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (!Subsystem) return;

	UEnhancedInputComponent* EnhancedInputComp = Cast<UEnhancedInputComponent>(InputComponent);
	if (!EnhancedInputComp) return;

	Subsystem->ClearAllMappings();
	if (NewMode == EControlMode::Menu) 
	{
		Subsystem->AddMappingContext(MenuInputMapping, MenuMappingPriority);
		SetInputMode(FInputModeUIOnly());
		bShowMouseCursor = true;
	}
	else if (NewMode == EControlMode::Aircraft) 
	{
		Subsystem->AddMappingContext(Mapping, AircraftMappingPriority);
		SetInputMode(FInputModeGameOnly());
		bShowMouseCursor = false;
	}
}

// TODO: Currently passing Menu States, will delegate this to purely menumanager

void AAircraftPlayerController::MenuBack() 
{
	print(text)
	if (!MenuManager) return;
	if (MenuHistory.Num() <= 0) return;
	MenuManager->GoBack(MenuHistory.Pop());
}

void AAircraftPlayerController::ManageMenuSetting(EMenuState NewMenu) 
{
	MenuHistory.Push(NewMenu);
	MenuManager->GetWidgetClassForState(NewMenu);
}

//Neutral = 50% Throttle, increase/decrease accordingly
void AAircraftPlayerController::Thrust(const FInputActionValue& Value)
{
	float thrustNeeded = FMath::Clamp(thrustPercentage + (Value.Get<float>() / 2.f), 0, 1.f);
	thrustPercentage = FMath::FInterpTo(thrustPercentage, thrustNeeded, GetWorld()->GetDeltaSeconds(), 2.f);
	isThrust = Value.Get<float>() != 0;
} 

//Movement Functions

void AAircraftPlayerController::Roll(const FInputActionValue& Value) 
{
	if (FlightComp) FlightComp->SetRoll(Value.Get<float>());
}

void AAircraftPlayerController::Pitch(const FInputActionValue& Value)
{
	if (FlightComp) FlightComp->SetPitch(Value.Get<float>());
}

void AAircraftPlayerController::Rudder(const FInputActionValue& Value) 
{
	if (FlightComp) FlightComp->SetYaw(Value.Get<float>());
}

//Specials

void AAircraftPlayerController::Weapons()
{
	if (!RadarComp || !WeaponComp || !FlightComp) return;
	if (WeaponComp->WeaponGroups.Num() > 0 && !CurrentWeaponClass)
	{
		TArray<TSubclassOf<ABaseWeapon>> Keys;
		WeaponComp->WeaponGroups.GetKeys(Keys);
		CurrentWeaponClass = Keys[0];
	}
	WeaponComp->FireWeaponSelected(CurrentWeaponClass, RadarComp->Selected,FlightComp->currentSpeed);
}

void AAircraftPlayerController::NextWeapon() 
{
	if (!WeaponComp) return;
	if (WeaponComp->WeaponGroups.Num() == 0) return;

	TArray<TSubclassOf<ABaseWeapon>> Keys;
	WeaponComp->WeaponGroups.GetKeys(Keys);

	int32 CurrentIndex = Keys.IndexOfByKey(CurrentWeaponClass);
	CurrentIndex = (CurrentIndex + 1) % Keys.Num();
	//if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("%d"),CurrentIndex));
	WeaponComp->SelectWeapon(CurrentIndex);
	CurrentWeaponClass = Keys[CurrentIndex];
}

void AAircraftPlayerController::PreviousWeapon() 
{
	if (!WeaponComp) return;
	if (WeaponComp->WeaponGroups.Num() == 0) return;

	TArray<TSubclassOf<ABaseWeapon>> Keys;
	WeaponComp->WeaponGroups.GetKeys(Keys);

	int32 CurrentIndex = Keys.IndexOfByKey(CurrentWeaponClass);
	CurrentIndex = (CurrentIndex - 1 + Keys.Num()) % Keys.Num();
	//if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("%d"), CurrentIndex));
	WeaponComp->SelectWeapon(CurrentIndex);
	CurrentWeaponClass = Keys[CurrentIndex];
}

void AAircraftPlayerController::Special() 
{
}

//Gun

void AAircraftPlayerController::ShootStart() 
{
	if (!fire) 
	{
		fire = true;
		Bullets();
		GetWorld()->GetTimerManager().SetTimer(RepeatTimerHandle, this, &AAircraftPlayerController::Bullets, 0.1f, true);
	}
}

void AAircraftPlayerController::ShootEnd() 
{
	fire = false;
	GetWorld()->GetTimerManager().ClearTimer(RepeatTimerHandle);
}

void AAircraftPlayerController::Bullets() 
{
	if (WeaponComp) WeaponComp->FireBullets();
}

//Camera Movement

void AAircraftPlayerController::Switch() 
{
	if (RadarComp) RadarComp->CycleTarget();
	if (Selected)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("Pitch: %s"), *Selected->GetName()));
	}
}

void AAircraftPlayerController::Focus()
{

}

void AAircraftPlayerController::FocusStop()
{

}

void AAircraftPlayerController::LookHor(const FInputActionValue& ValueX) 
{
	if (!ManagerComp) return;
	ManagerComp->LookHor(ValueX.Get<float>());
}

void AAircraftPlayerController::LookVer(const FInputActionValue& ValueY) 
{
	if (!ManagerComp) return;
	ManagerComp->LookVer(ValueY.Get<float>());
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
	if (CurrentMode == EControlMode::Menu) return;
	if (!isThrust) thrustPercentage = FMath::FInterpTo(thrustPercentage, 0.5, DeltaSeconds, 2.f);

	if(FlightComp) FlightComp->SetThrust(thrustPercentage);
	
	if (RadarComp) 
	{
		if (HUD)
		{
			ABaseAircraft* Temp = Cast<ABaseAircraft>(RadarComp->Selected);
			if (Temp) 
			{
				HUD->Targets.Add(Temp);
			}
		}
	}
	Super::Tick(DeltaSeconds);
}