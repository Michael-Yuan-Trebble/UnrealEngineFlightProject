// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("CONTROL!"));
#include "AircraftPlayerController.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/World.h"
#include "Math/UnrealMathUtility.h"
#include "cmath"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "TimerManager.h"
#include "BaseAircraft.h"

bool isRoll = false;
bool isPitch = false;
bool isYaw = false;
bool isThrust = false;
float targetSpringArm;
float prevPitch;
float prevYaw;
float thrustNeeded;

void AAircraftPlayerController::BeginPlay() 
{

	if (GetLocalPlayer())
	{
		UEnhancedInputLocalPlayerSubsystem* InputSubsystem = GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();

		if (GetPawn()) 
		{
			ABaseAircraft* Base = Cast<ABaseAircraft>(GetPawn());
			if (Base) 
			{

				//Get Some Variables from controlled Aircraft

				SpringArm = Base->GetSpringArm();
				CameraComp = Base->GetCamera();
				springArmLengthOriginal = Base->ReturnSpringArmLength();
				planeAcceleration = Base->ReturnAcceleration();
				maxSpeed = Base->ReturnMaxSpeed();
				turnRate = Base->ReturnTurnRate();
				roll = Base->ReturnRollRate();
				rudderRate = Base->ReturnRudder();
				SpringArm = Base->GetSpringArm();
				Airframe = Base->GetMesh();
				PS = Cast<ACurrentPlayerState>(this->PlayerState);

				//Find weapon selection and put it in (Currently set values)
				
				if (PS) 
				{
					Base->EquipWeapons(PS->SelectedWeapons);
				}
			}
		}

		if (InputSubsystem)
		{
			InputSubsystem->AddMappingContext(Mapping, 0);
		}
	}
	power = (log10(20 / (0.07 * 1.225))) / (log10(maxSpeed));

	Super::BeginPlay();
}

//Large amount of input binding
void AAircraftPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	if (UEnhancedInputComponent* EnhancedInputComp = Cast<UEnhancedInputComponent>(InputComponent)) 
	{
		if (Throttle) 
		{
			EnhancedInputComp->BindAction(Throttle, ETriggerEvent::Triggered, this, &AAircraftPlayerController::Thrust);
			EnhancedInputComp->BindAction(Throttle, ETriggerEvent::Completed, this, &AAircraftPlayerController::Thrust);
		}
		if (IA_Roll)
		{
			EnhancedInputComp->BindAction(IA_Roll, ETriggerEvent::Triggered, this, &AAircraftPlayerController::Roll);
			EnhancedInputComp->BindAction(IA_Roll, ETriggerEvent::Completed, this, &AAircraftPlayerController::Roll);
		}
		if (IA_Pitch) 
		{
			EnhancedInputComp->BindAction(IA_Pitch, ETriggerEvent::Triggered, this, &AAircraftPlayerController::Pitch);
			EnhancedInputComp->BindAction(IA_Pitch, ETriggerEvent::Completed, this, &AAircraftPlayerController::Pitch);
		}
		if (IA_Rudder) {
			EnhancedInputComp->BindAction(IA_Rudder, ETriggerEvent::Triggered, this, &AAircraftPlayerController::Rudder);
			EnhancedInputComp->BindAction(IA_Rudder, ETriggerEvent::Completed, this, &AAircraftPlayerController::Rudder);
		}
		if (IA_Special) 
		{
			EnhancedInputComp->BindAction(IA_Special, ETriggerEvent::Started, this, &AAircraftPlayerController::Special);
		}
		if (IA_Shoot) 
		{
			EnhancedInputComp->BindAction(IA_Shoot, ETriggerEvent::Started, this, &AAircraftPlayerController::ShootStart);
			EnhancedInputComp->BindAction(IA_Shoot, ETriggerEvent::Completed, this, &AAircraftPlayerController::ShootEnd);
		}
		if (IA_Weapons)
		{
			EnhancedInputComp->BindAction(IA_Weapons, ETriggerEvent::Started, this, &AAircraftPlayerController::Weapons);
		}
		if (IA_LookX) 
		{
			EnhancedInputComp->BindAction(IA_LookX, ETriggerEvent::Triggered, this, &AAircraftPlayerController::LookHor);
			EnhancedInputComp->BindAction(IA_LookX, ETriggerEvent::Completed, this, &AAircraftPlayerController::LookHor);
		}
		if (IA_LookY) 
		{
			EnhancedInputComp->BindAction(IA_LookY, ETriggerEvent::Triggered, this, &AAircraftPlayerController::LookVer);
			EnhancedInputComp->BindAction(IA_LookY, ETriggerEvent::Completed, this, &AAircraftPlayerController::LookVer);
		}
		if (IA_Focus) 
		{
			EnhancedInputComp->BindAction(IA_Focus, ETriggerEvent::Started, this, &AAircraftPlayerController::Focus);
			EnhancedInputComp->BindAction(IA_Focus, ETriggerEvent::Ongoing, this, &AAircraftPlayerController::Focus);
			EnhancedInputComp->BindAction(IA_Focus, ETriggerEvent::Completed, this, &AAircraftPlayerController::FocusStop);
		}
		if (IA_Switch) 
		{
			EnhancedInputComp->BindAction(IA_Switch, ETriggerEvent::Started, this, &AAircraftPlayerController::Switch);
		}
		if (IA_Zoom) 
		{
			EnhancedInputComp->BindAction(IA_Zoom, ETriggerEvent::Started, this, &AAircraftPlayerController::MapZoom);
			EnhancedInputComp->BindAction(IA_Zoom, ETriggerEvent::Ongoing, this, &AAircraftPlayerController::MapZoom);
			EnhancedInputComp->BindAction(IA_Zoom, ETriggerEvent::Completed, this, &AAircraftPlayerController::StopMapZoom);
		}
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
	Cast<ABaseAircraft>(GetPawn())->InputPitchValue = inputPitch;
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
	if (GetPawn()) {
		if (Cast<ABaseAircraft>(GetPawn())) {
			//When can switch change to CurrentWeaponIndex
			Cast<ABaseAircraft>(GetPawn())->FireWeapon(1);
		}
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
	*/

	/*
	Logistics Formula
	drag = 15/(1+pow(2,-0.1(prevSpeed-650)));
	currentSpeed = currentSpeed + currentThrust - drag;
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
	vectorLocation = (currentSpeed)*Cast<ABaseAircraft>(GetPawn())->GetActorForwardVector();
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

	SpeedAdd(thrustPercentage, currentSpeed);

	Super::Tick(DeltaSeconds);
}