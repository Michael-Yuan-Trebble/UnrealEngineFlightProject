// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("AH!"));
#include "AircraftPlayerController.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/World.h"
#include "Math/UnrealMathUtility.h"
#include "EnhancedInput/Public/InputMappingContext.h"
#include "GameFramework/SpectatorPawn.h"
#include "cmath"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "F16AI.h"
#include "TimerManager.h"
#include "BaseAircraft.h"

AAircraftPlayerController::AAircraftPlayerController() 
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	static ConstructorHelpers::FClassFinder<ULockBoxWidget> WidgetClassFinder(TEXT("/Game/UI/MyLockBoxWidget"));
	if (WidgetClassFinder.Succeeded()) 
	{
		LockBoxWidgetClasses = WidgetClassFinder.Class;
	}
}

void AAircraftPlayerController::BeginPlay() 
{
	Super::BeginPlay();

	power = (log10(20 / (0.07 * 1.225))) / (log10(maxSpeed));

	if (GetLocalPlayer()) 
	{
		UEnhancedInputLocalPlayerSubsystem* InputSubsystem = GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
		Mapping = LoadObject<UInputMappingContext>(nullptr, TEXT("/Game/Controls/Mapping"));
		if (InputSubsystem) 
		{
			InputSubsystem->AddMappingContext(Mapping, 0);
		}
	}
}

void AAircraftPlayerController::OnPossess(APawn* InPawn) 
{
	if (InPawn)
	{
		Controlled = Cast<ABaseAircraft>(InPawn);
		if (Controlled)
		{
			//Get Some Variables from controlled Aircraft
			SpringArm = Controlled->GetSpringArm();
			CameraComp = Controlled->GetCamera();
			springArmLengthOriginal = Controlled->ReturnSpringArmLength();
			planeAcceleration = Controlled->ReturnAcceleration();
			maxSpeed = Controlled->ReturnMaxSpeed();
			turnRate = Controlled->ReturnTurnRate();
			roll = Controlled->ReturnRollRate();
			rudderRate = Controlled->ReturnRudder();
			SpringArm = Controlled->GetSpringArm();
			Airframe = Controlled->GetMesh();

			if (CameraComp) 
			{
				SetViewTarget(InPawn);
			}
		}
	}
}

//Large amount of input binding
void AAircraftPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	if (UEnhancedInputComponent* EnhancedInputComp = Cast<UEnhancedInputComponent>(InputComponent)) 
	{
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
				
				ULockBoxWidget* LockOnWidget = nullptr;
				if (!ActiveLockOnWidgets.Contains(RegisteredPawn)) 
				{
					//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("Pitch: %s"), *RegisteredPawn->GetName()));
					LockOnWidget = CreateWidget<ULockBoxWidget>(this, LockBoxWidgetClasses);
					if (LockOnWidget)
					{
						LockOnWidget->AddToViewport();
						ActiveLockOnWidgets.Add(RegisteredPawn, LockOnWidget);
						LockOnWidget->SetAlignmentInViewport(FVector2D(0.5f, 0.5f));
					}
				}
				else 
				{
					LockOnWidget = ActiveLockOnWidgets[RegisteredPawn];
				}

				if (LockOnWidget) 
				{
					FVector2D ScreenPosition;
					FVector WorldPosition = RegisteredPawn->GetActorLocation() + FVector(0.f,0.f,0.f);
					if (this->ProjectWorldLocationToScreen(WorldPosition, ScreenPosition,true)) 
					{
						//FVector2D WidgetSize(64.f, 64.f);
						//ScreenPosition -= WidgetSize * 0.5f;
						LockOnWidget->SetPositionInViewport(ScreenPosition, false);
						float Distance = FVector::Dist(RegisteredPawn->GetActorLocation(), CameraComp->GetComponentLocation());
						float FOV = this->PlayerCameraManager->GetFOVAngle();
						float FOVFactor = FMath::Tan(FMath::DegreesToRadians(FOV * 0.5f));

						float ScaleFactor = (1000.f / Distance) * FOVFactor;
						ScaleFactor = FMath::Clamp(ScaleFactor, 0.1f, 0.3f);

						LockOnWidget->SetRenderScale(FVector2D(ScaleFactor, ScaleFactor));
					}
				}
			}
		}
	}

	for (auto It = ActiveLockOnWidgets.CreateIterator(); It; ++It) 
	{
		if (!Detected.ContainsByPredicate([&](const FDetectedAircraftInfo& Info) {return Info.CurrentPawn == It.Key(); })) 
		{
			if (It.Value()) 
			{
				It.Value()->RemoveFromParent();
			}
			It.RemoveCurrent();
		}
	}
}

bool AAircraftPlayerController::GetTargetScreenPosition(AActor* Target, FVector2D& OutScreenPos) const 
{
	if (!IsValid(Target)) return false;
	return ProjectWorldLocationToScreen(Target->GetActorLocation(), OutScreenPos);
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
	currentRoll = FMath::FInterpTo(currentRoll, inputRoll, GetWorld()->GetDeltaSeconds(), 8.f);
	currentRoll = FMath::Clamp(currentRoll, -roll, roll);
	isRoll = true;

	if (inputRoll == 0) 
	{
		isRoll = false;
	}
}

void AAircraftPlayerController::Pitch(const FInputActionValue& Value)
{
	inputPitch = Value.Get<float>();
	//Cast<ABaseAircraft>(GetPawn())->InputPitchValue = inputPitch;
	currentPitch = FMath::FInterpTo(currentPitch, inputPitch, GetWorld()->GetDeltaSeconds(), 8.f);
	currentPitch = FMath::Clamp(currentPitch, -turnRate, turnRate);

	if (inputPitch < 0.01f) {
		currentPitch = currentPitch * 0.7f;
	}

	isPitch = true;
	if (inputPitch == 0) {
		isPitch = false;
	}
}

void AAircraftPlayerController::Rudder(const FInputActionValue& Value) 
{
	inputYaw = Value.Get<float>();
	isYaw = true;

	currentRudder = FMath::FInterpTo(currentRudder, inputYaw, GetWorld()->GetDeltaSeconds(), 2.f);
	currentRudder = FMath::Clamp(currentRudder, -rudderRate, rudderRate);
	if (inputYaw == 0)
	{
		isYaw = false;
	}
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
	print(text);
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

//Velocity

void AAircraftPlayerController::SpeedAdd(float ThrustPercentage,float prevSpeed) 
{
	/*
	Drag Formula
	power = (log10(20 / (0.07 * 1.225)))/(log10(maxSpeed));
	float prevSpeed = currentSpeed;
	drag = pow(prevSpeed, power);
	currentSpeed = currentSpeed + (currentThrust - ((0.5 * 0.07 * 1.225) * pow(prevSpeed, power)));


	Air Density and Drag

	Have A Logistic Formula in order to calculate Air Density, some altitude like 15000 will be the "cap"

	Logistics Formula
	drag = 15/(1+pow(2,-0.1(prevSpeed-650)));
	currentSpeed = currentSpeed + currentThrust - drag;
	
	Some More Drag Stuff

	Drag = 0.5 * AirDensity * Speed² * DragCoefficient * Area
	DragForce = -Velocity * DragCoefficient
	
	AOA Information

	For each aircraft, have a different maximum AOA angle, AOA drag mult, Stall Threshold AOA (This will only come into play due to low airspeed if at all)

	Some AOA code
	FVector Forward = GetActorForwardVector();
	FVector VelocityDir = CurrentVelocity.GetSafeNormal();

	float AoA = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(Forward, VelocityDir)));

	float AoAAbs = FMath::Abs(AoA);
	float DragMultiplier = 1.0f;

	if (AoAAbs > MaxOptimalAoA)
	{
		float OverAoA = FMath::Clamp((AoAAbs - MaxOptimalAoA) / (MaxAoABeforeHardPenalty - MaxOptimalAoA), 0.0f, 1.0f);
		DragMultiplier += OverAoA * AoADragMultiplier;
	}

	Stall (if wanted)

	if (AoAAbs >= StallThresholdAoA)
	{
		bIsStalling = true;
		// Maybe reduce lift or cap velocity
	}
	else
	{
		bIsStalling = false;
	}

	*/

	//Needs to decrease by plane acceleration when input thrust from before is higher than what it currently is
	if (ThrustPercentage <= 0.f) {
		currentSpeed = currentSpeed - planeAcceleration;
	}
	else {
		currentSpeed = currentSpeed + ((planeAcceleration * thrustPercentage) - ((0.5 * 0.07 * 1.225) * pow(prevSpeed, power)));
	}
	
	currentSpeed = FMath::Clamp(currentSpeed, 0, maxSpeed);
	currentSpeed = FMath::FInterpTo(prevSpeed, currentSpeed, GetWorld()->GetDeltaSeconds(), 2.f);

	if (Controlled && IsValid(Controlled)) {
		vectorLocation = (currentSpeed)*Controlled->GetActorForwardVector();
		Controlled->AddActorWorldOffset(vectorLocation);
		Controlled->currentSpeed = currentSpeed;
	}
}

//Might Make two states so Tick doesn't have to do as much, one grounded and one not
void AAircraftPlayerController::Tick(float DeltaSeconds) 
{
	//Make transitioning to 0 smoother

	if (!isFlying && currentSpeed > 10.f) isFlying = true;
	if (!isThrust) thrustPercentage = FMath::FInterpTo(thrustPercentage, 0.5, DeltaSeconds, 2.f);
	if (!isRoll) currentRoll = FMath::FInterpTo(currentRoll, 0, DeltaSeconds, 3.5f);
	if (!isPitch) currentPitch = FMath::FInterpTo(currentPitch, 0, DeltaSeconds, 10.f);
	if (!isYaw) currentRudder = FMath::FInterpTo(currentRudder, 0, DeltaSeconds, 7.f);
	if (SpringArm) 
	{
		targetSpringArm = springArmLengthOriginal + (50 * (0.5 - thrustPercentage));
		SpringArm->TargetArmLength = FMath::FInterpTo(SpringArm->TargetArmLength, targetSpringArm, DeltaSeconds, 2.f);
	}

	if (CameraComp) {
		//TrackTarget();
	}

	SpeedAdd(thrustPercentage, currentSpeed);

	//Apply rotations as Quat and different movement if Flying vs not
	if(Controlled)
	{
		if (isFlying) 
		{
			FQuat DeltaRotation = FQuat(FRotator(currentPitch, currentRudder, currentRoll));
			Controlled->AddActorLocalRotation(DeltaRotation);
		}
		else 
		{
			FQuat DeltaRotation = FQuat(FRotator(0, currentRudder, 0));
			Controlled->AddActorLocalRotation(DeltaRotation);
		}
		ScanTargets();
	}
	Super::Tick(DeltaSeconds);
}