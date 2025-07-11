// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("AH!"));
#include "AircraftPlayerController.h"
#include "InputMappingContext.h"
#include "Engine/World.h"
#include "Math/UnrealMathUtility.h"
#include "EnhancedInput/Public/InputMappingContext.h"
#include "GameFramework/SpectatorPawn.h"
#include "cmath"
#include "MenuManagerComponent.h"
#include "Gamemodes/CurrentPlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Aircraft/AI/F16AI.h"
#include "UI/PlayerHUD.h"
#include "TimerManager.h"
#include "Aircraft/BaseAircraft.h"

AAircraftPlayerController::AAircraftPlayerController() 
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

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

	MenuManager = CreateDefaultSubobject<UMenuManagerComponent>(TEXT("MenuManager"));
}

void AAircraftPlayerController::BeginPlay() 
{
	Super::BeginPlay();
	ACurrentPlayerState* PS = Cast<ACurrentPlayerState>(this->PlayerState);
	MenuManager->InitializePC(this, PS);
}

void AAircraftPlayerController::OnPossess(APawn* InPawn) 
{
	if (!InPawn) return;
	Controlled = Cast<ABaseAircraft>(InPawn);

	if (!Controlled) return;
	//Get Some Variables from controlled Aircraft
	SpringArm = Controlled->GetSpringArm();
	CameraComp = Controlled->GetCamera();
	SpringArm = Controlled->GetSpringArm();
	Airframe = Controlled->GetMesh();

	if (!CameraComp) return;
	SetViewTarget(InPawn);

	APlayerHUD* HUD = Cast<APlayerHUD>(GetHUD());
	if (!HUD) return;
	HUD->PC = this;
}

void AAircraftPlayerController::BindAircraftInputs(UEnhancedInputComponent* EnhancedInputComp) {
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
		if (IA_Rudder) {
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

void AAircraftPlayerController::BindMenuInputs(UEnhancedInputComponent* EnhancedInputComp) {
	if (!EnhancedInputComp) return;
	FSoftObjectPath UpPath(TEXT("/Game/Controls/MenuInputs/Up.Up"));
	TSoftObjectPtr<UInputAction> SoftUp(UpPath);
	Up = SoftUp.LoadSynchronous();
	if (Up)
	{
		print(text)
		//EnhancedInputComp->BindAction(Up, ETriggerEvent::Triggered, this, &AAircraftPlayerController::Thrust);
		//EnhancedInputComp->BindAction(Up, ETriggerEvent::Completed, this, &AAircraftPlayerController::Thrust);
	}
	FSoftObjectPath DownPath(TEXT("/Game/Controls/MenuInputs/Down.Down"));
	TSoftObjectPtr<UInputAction> SoftDown(DownPath);
	Down = SoftDown.LoadSynchronous();
	if (Down)
	{
		print(text)
		//EnhancedInputComp->BindAction(Up, ETriggerEvent::Triggered, this, &AAircraftPlayerController::Thrust);
		//EnhancedInputComp->BindAction(Up, ETriggerEvent::Completed, this, &AAircraftPlayerController::Thrust);
	}
	FSoftObjectPath BackPath(TEXT("/Game/Controls/MenuInputs/Back.Back"));
	TSoftObjectPtr<UInputAction> SoftBack(BackPath);
	IA_Back = SoftBack.LoadSynchronous();
	if (IA_Back)
	{
		print(text)
			//EnhancedInputComp->BindAction(Up, ETriggerEvent::Triggered, this, &AAircraftPlayerController::Thrust);
			//EnhancedInputComp->BindAction(Up, ETriggerEvent::Completed, this, &AAircraftPlayerController::Thrust);
	}
}

void AAircraftPlayerController::SetupInputComponent() 
{
	Super::SetupInputComponent();
	if (UEnhancedInputComponent* EnhancedInputComp = Cast<UEnhancedInputComponent>(InputComponent)) 
	{
		BindAircraftInputs(EnhancedInputComp);
		BindMenuInputs(EnhancedInputComp);
	}
}

void AAircraftPlayerController::SetControlMode(EControlMode NewMode) 
{
	if (CurrentMode == NewMode || !GetLocalPlayer()) return;
	CurrentMode = NewMode;

	auto* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());

	if (!Subsystem) return;

	auto* EnhancedInputComp = Cast<UEnhancedInputComponent>(InputComponent);

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

void AAircraftPlayerController::MenuBack() 
{
	if (!MenuManager) return;
	MenuManager->GoBack();
}

void AAircraftPlayerController::ManageMenuSetting(EMenuState NewMenu) 
{
	MenuHistory.Push(CurrentMenuState);
	CurrentMenuState = NewMenu;
	if (CurrentWidget) 
	{
		CurrentWidget->RemoveFromParent();
		CurrentWidget = nullptr;
	}
	MenuManager->GetWidgetClassForState(NewMenu);
}

void AAircraftPlayerController::SetWidget(UUserWidget* Widget) 
{
	CurrentWidget = Widget;
}

void AAircraftPlayerController::ScanTargets() 
{
	Detected.Empty();
	TArray<AActor*> AllAircraft;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APawn::StaticClass(), AllAircraft);

	for (AActor* Target : AllAircraft)
	{
		APawn* RegisteredPawn = Cast<APawn>(Target);
		if (RegisteredPawn && RegisteredPawn != Controlled && !RegisteredPawn->IsA(ASpectatorPawn::StaticClass()))
		{
			FDetectedAircraftInfo TempInfo;
			TempInfo.Location = RegisteredPawn->GetActorLocation();
			TempInfo.Rotation = RegisteredPawn->GetActorRotation();
			TempInfo.threatLevel = TempInfo.CalculateThreat();

			if (TempInfo.threatLevel > 0)
			{
				TempInfo.CurrentPawn = RegisteredPawn;
				Detected.Add(TempInfo);
			}
		}
	}
}

const TArray<FDetectedAircraftInfo>& AAircraftPlayerController::GetDetectedTargets() const
{
	return Detected;
}

void AAircraftPlayerController::CycleTarget() 
{
	FVector CameraLoc;
	FRotator CameraRot;
	GetPlayerViewPoint(CameraLoc, CameraRot);
	FVector Forward = CameraRot.Vector();

	float BestDot = -1.0f;
	AActor* ClosestTarget = nullptr;

	for (FDetectedAircraftInfo Target : Detected)
	{	
		AActor* temp = Cast<AActor>(Target.CurrentPawn);
		if (!IsValid(temp)) continue;

		FVector ToTarget = (temp->GetActorLocation() - CameraLoc).GetSafeNormal();
		float Dot = FVector::DotProduct(Forward, ToTarget);

		if (Dot > BestDot)
		{
			BestDot = Dot;
			ClosestTarget = temp;
		}
	}

	if (ClosestTarget != Selected && ClosestTarget != nullptr)
	{
		Selected = ClosestTarget;
		Controlled->Tracking = ClosestTarget;
	}
	else 
	{
		CycleToNextTarget();
	}
}

void AAircraftPlayerController::CycleToNextTarget() 
{
	if (Detected.Num() == 0) return;

	FVector CameraLoc;
	FRotator CameraRot;
	GetPlayerViewPoint(CameraLoc, CameraRot);
	FVector Forward = CameraRot.Vector();

	float BestAngleDiff = FLT_MAX;
	AActor* BestTarget = nullptr;

	for (FDetectedAircraftInfo Target : Detected)
	{
		AActor* temp = Cast<AActor>(Target.CurrentPawn);

		if (!IsValid(temp) || temp == Selected) continue;

		FVector ToTarget = (temp->GetActorLocation() - CameraLoc).GetSafeNormal();
		float AngleDiff = FMath::Acos(FVector::DotProduct(Forward, ToTarget));

		if (AngleDiff < BestAngleDiff)
		{
			BestAngleDiff = AngleDiff;
			BestTarget = temp;
		}
	}

	if (BestTarget)
	{
		Selected = BestTarget;
		Controlled->Tracking = BestTarget;
		//VFX here
	}
}

//Neutral = 50% Throttle, increase/decrease accordingly
void AAircraftPlayerController::Thrust(const FInputActionValue& Value)
{
	inputThrust = Value.Get<float>();
	thrustNeeded = FMath::Clamp(thrustPercentage + (inputThrust / 2.f), 0, 1.f);
	thrustPercentage = FMath::FInterpTo(thrustPercentage, thrustNeeded, GetWorld()->GetDeltaSeconds(), 2.f);
	isThrust = true;
	if (inputThrust == 0)
	{
		isThrust = false;
	}
} 

//Movement Functions

void AAircraftPlayerController::Roll(const FInputActionValue& Value) 
{
	inputRoll = Value.Get<float>();
	Controlled->SetRoll(inputRoll);
}

void AAircraftPlayerController::Pitch(const FInputActionValue& Value)
{
	inputPitch = Value.Get<float>();
	Controlled->SetPitch(inputPitch);
}

void AAircraftPlayerController::Rudder(const FInputActionValue& Value) 
{
	inputYaw = Value.Get<float>();
	Controlled->SetYaw(inputYaw);
}

//Specials

void AAircraftPlayerController::Weapons()
{
	//Different states on selected or not
	if (Controlled && !Selected) 
	{
		//When can switch change to CurrentWeaponIndex
		Controlled->FireWeaponNotSelected(1);
	}
	else if (Controlled && Selected) 
	{
		Controlled->FireWeaponSelected(1, Selected);
	}
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
	if (Controlled) {
		Controlled->FireBullets();
	}
}

//Camera Movement

void AAircraftPlayerController::Switch() 
{
	print(text)
	CycleTarget();
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
	lookX = ValueX.Get<float>();

	if(FMath::Abs(lookX)==0.f)
	{
		lookX = 0;
	}

	prevYaw = currentYaw;
	currentYaw += lookX;

	if (currentYaw >= 180)
	{
		lookX = 180 - prevYaw;
		currentYaw = 180;
	}
	if (currentYaw <= -180)
	{
		lookX = 180 - prevYaw;
		currentYaw = -180;
	}

	SpringArm->AddRelativeRotation(FRotator(0.f, lookX, 0.f));
}

void AAircraftPlayerController::LookVer(const FInputActionValue& ValueY) 
{
	lookY = ValueY.Get<float>();

	if (FMath::Abs(lookY)==0.f) 
	{
		lookY = 0;
	}

	prevPitch = seePitch;
	seePitch += lookY;

	if (seePitch >= 85) 
	{
		lookY = 85 - prevPitch;
		seePitch = 85;
	}
	if (seePitch <= -85)
	{
		lookY = 85 + prevPitch;
		seePitch = -85;
	}
	SpringArm->AddRelativeRotation(FRotator(lookY, 0.f, 0.f));
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
	//Make transitioning to 0 smoother
	if (!isThrust) thrustPercentage = FMath::FInterpTo(thrustPercentage, 0.5, DeltaSeconds, 2.f);

	if(Controlled)
	{
		Controlled->ApplySpeed(thrustPercentage,DeltaSeconds);
		Controlled->AdjustSpringArm(DeltaSeconds, thrustPercentage);
		Controlled->ApplyRot(DeltaSeconds);
		ScanTargets();
	}
	Super::Tick(DeltaSeconds);
}