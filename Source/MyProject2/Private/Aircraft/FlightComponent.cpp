// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Flight Component!"));
#include "Aircraft/FlightComponent.h"

//power = (log10(20 / (0.07 * 1.225))) / (log10(ListedMaximumSpeed));

enum class ThrottleStage
{
	Slow,
	Normal,
	Afterburner
};

ThrottleStage getThrottleStage(float throttle)
{
	// Throttle Stages for pronounced acceleration differences

	throttle *= 100.f;
	if (throttle >= 0 && throttle <= 40) return ThrottleStage::Slow;
	else if (throttle <= 80) return ThrottleStage::Normal;
	else return ThrottleStage::Afterburner;
}


UFlightComponent::UFlightComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UFlightComponent::Setup(ABaseAircraft* InControl, UAircraftStats* InStats)
{
	Controlled = InControl;
	AircraftStats = InStats;
}

void UFlightComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	if (!isFlying && currentSpeed > 10.f) isFlying = true;
	if (!Controlled || !AircraftStats) return;

	// ====================================
	// Movement Application Components
	// ====================================

	ApplySpeed(CurrentThrust, DeltaTime);
	RollAOA(DeltaTime);
	ApplyRot(DeltaTime);
	ReturnAOA(DeltaTime);

	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UFlightComponent::ApplySpeed(float ThrottlePercentage, float DeltaSeconds)
{
	ThrottleStage currentStage = getThrottleStage(ThrottlePercentage);
	switch (currentStage)
	{
	case ThrottleStage::Slow: SlowSpeed(ThrottlePercentage); break;
	case ThrottleStage::Normal: NormalSpeed(ThrottlePercentage); break;
	case ThrottleStage::Afterburner: AfterburnerSpeed(ThrottlePercentage);break;
	}
	AdjustSpringArm(DeltaSeconds, ThrottlePercentage);
	float totalFlightPercent = 0;
	float drag = 0;
	float trueAcceleration = 0;

	// Target speed for better acceleration until a certain point feeling

	if (targetSpeed == 0)
	{
		trueAcceleration = Acceleration;
	}
	else
	{
		totalFlightPercent = currentSpeed / targetSpeed;
		if (totalFlightPercent >= 0.8)
		{
			drag = (Acceleration * 2) / (1.f + FMath::Pow(2.f, -0.1f * (currentSpeed - targetSpeed)));
		}
		trueAcceleration = Acceleration - drag;
	}

	// TODO: Apply AOA drag into acceleration calculation, fine tune numbers

	FVector F = Controlled->Airframe->GetForwardVector().GetSafeNormal();
	FVector VelocityDir = Velocity.GetSafeNormal();

	float CosTheta = FVector::DotProduct(F, VelocityDir);

	CosTheta = FMath::Clamp(CosTheta, -1.0f, 1.0f);

	float AOA = FMath::RadiansToDegrees(FMath::Acos(CosTheta));

	FVector Cross = FVector::CrossProduct(F, VelocityDir);
	float Sign = (Cross.Z >= 0.f) ? 1.f : -1.f;

	AOA *= Sign;

	float t = DragAOA(AOA);

	currentSpeed += trueAcceleration;
	currentSpeed = FMath::Clamp(currentSpeed, 0, AircraftStats->MaxSpeed);

	if (GEngine)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Current Speed: %.2f"), trueAcceleration));
	}

	float InterpSpeed = FMath::FInterpTo(Velocity.Size(), currentSpeed, DeltaSeconds, 2.f);

	FVector Forward = Controlled->GetActorForwardVector();

	Velocity = Forward * InterpSpeed;

	Controlled->AddActorWorldOffset(Velocity * DeltaSeconds, true);

	// ====================================
	// Draw AOA lines
	// Blue = Root forward
	// Red = Mesh Forward
	// ====================================

	DrawDebugLine(GetWorld(), Controlled->GetActorLocation(), Controlled->GetActorLocation() + Forward * 300, FColor::Blue, false, 0.f, 0, 2.f);
	DrawDebugLine(GetWorld(), Controlled->GetActorLocation(), Controlled->GetActorLocation() + Controlled->Airframe->GetForwardVector() * 300, FColor::Green, false, 0.f, 0, 2.f);
}

// ====================================
// Throttle Stage for Acceleration and Target Speed
// ====================================

void UFlightComponent::SlowSpeed(float ThrottlePercentage)
{
	float negation = 1 - ThrottlePercentage;
	if (ThrottlePercentage <= 0.1)
	{
		Acceleration = -(AircraftStats->Thrust * negation * 5);
		targetSpeed = 0;
	}
	else
	{
		Acceleration = -(AircraftStats->Thrust * negation * 2);
		targetSpeed = 0;
	}
}

void UFlightComponent::NormalSpeed(float ThrottlePercentage)
{
	Acceleration = AircraftStats->Thrust * ThrottlePercentage;
	targetSpeed = AircraftStats->MaxSpeed * 0.75;
}

void UFlightComponent::AfterburnerSpeed(float ThrottlePercentage)
{
	if (ThrottlePercentage >= 0.9)
	{
		Acceleration = AircraftStats->Thrust * ThrottlePercentage * 5;
		targetSpeed = AircraftStats->MaxSpeed;
	}
	else
	{
		Acceleration = AircraftStats->Thrust * ThrottlePercentage * 2;
		targetSpeed = AircraftStats->MaxSpeed * 0.9;
	}
}

// ====================================
// Turning/Rotation Calculations
// ====================================

void UFlightComponent::ApplyRot(float DeltaSeconds)
{
	if (!Controlled || !AircraftStats) return;

	// ====================================
	// Apply User Inputs
	// ====================================

	ApplyYaw(DeltaSeconds);
	ApplyRoll(DeltaSeconds);
	ApplyPitch(DeltaSeconds);

	ReturnAOA(DeltaSeconds);
}

void UFlightComponent::ReturnAOA(float DeltaSeconds)
{
	// Rotates the Root vector toward Airframe's vector
	FQuat CurrentQuat = Controlled->GetActorQuat();
	FQuat TargetQuat = Controlled->Airframe->GetComponentQuat();
	float RootTurnSpeed = 2.0f;
	float RootAlpha = FMath::Clamp(RootTurnSpeed * DeltaSeconds, 0.f, 1.f);
	FQuat NewRootQuat = FQuat::Slerp(CurrentQuat, TargetQuat, RootAlpha);
	Controlled->SetActorRotation(NewRootQuat);

	// Resets the Airframe's vector 
	FQuat AirframeCurrentRelQuat = Controlled->Airframe->GetRelativeRotation().Quaternion();
	FQuat IdentityQuat = FQuat::Identity;
	float NoseAlpha = FMath::Clamp(AircraftStats->AOARecoverySpeed * DeltaSeconds, 0.f, 1.f);
	FQuat NewRelQuat = FQuat::Slerp(AirframeCurrentRelQuat, IdentityQuat, NoseAlpha);
	Controlled->Airframe->SetRelativeRotation(NewRelQuat.Rotator());
}

void UFlightComponent::AdjustSpringArm(float DeltaSeconds, float ThrottlePercentage)
{
	if (!AircraftStats) return;
	//TODO: Move SpringArm based on User's throttle input
	//Controlled->SpringArm->TargetArmLength = FMath::FInterpTo(Controlled->SpringArm->TargetArmLength, AircraftStats->SpringArmLength + (50 * (0.5 - ThrottlePercentage)), DeltaSeconds, 2.f);
}

float UFlightComponent::DragAOA(float AOA)
{
	AOA = FMath::Abs(AOA);
	float AOARadians = FMath::DegreesToRadians(AOA);
	float drag = 0.75 * FMath::Pow(AOARadians, 2);
	//if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("%f"), AOA));
	return drag;
}

void UFlightComponent::RollAOA(float DeltaSeconds)
{
	// ====================================
	// Apply a downward AOA based on user's roll orientation with the ground
	// ====================================

	FVector Forward = Controlled->Airframe->GetForwardVector();
	FVector Up = Controlled->Airframe->GetUpVector();

	float PitchAngle = FMath::RadiansToDegrees(
		FMath::Atan2(Forward.Z, FVector(Forward.X, Forward.Y, 0.f).Size())
	);

	float UpDot = FVector::DotProduct(Up, FVector::UpVector);

	float DownPitchStrength = FMath::Clamp(1.f - UpDot, 0.f, 2.f) * 2.f;

	DownPitch = -DownPitchStrength * DeltaSeconds;

	FVector WorldPitchAxis = FVector::CrossProduct(Forward, FVector::UpVector).GetSafeNormal();

	// Prevent potential Gimbal Lock, investigating further if it is truly needed
	if (PitchAngle > -85.f)
	{
		FQuat Rotate = FQuat(WorldPitchAxis, FMath::DegreesToRadians(DownPitch));
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Pitch: %.2f"), DownPitch));
		Controlled->Airframe->AddWorldRotation(Rotate);
	}
}

// ====================================
// Apply Rotations
// ====================================

void UFlightComponent::ApplyPitch(float DeltaSeconds)
{
	float InterpSpeed = 0;
	if (Controlled->GetController() && Controlled->GetController()->IsPlayerController())
	{
		InterpSpeed = AircraftStats->TurnRate * 50;
		float TargetPitchRate = UserPitch * InterpSpeed;
		NextPitch = FMath::FInterpTo(NextPitch, TargetPitchRate, DeltaSeconds, 5.f);

		FRotator DeltaRot(NextPitch * DeltaSeconds, 0.f, 0.f);
		Controlled->Airframe->AddLocalRotation(DeltaRot);
	}
	else
	{
		InterpSpeed = AircraftStats->TurnRate * 10;
		float TargetPitchAngle = UserPitch * InterpSpeed;
		FRotator CurrentRot = Controlled->Airframe->GetRelativeRotation();
		float NewPitch = FMath::FInterpTo(CurrentRot.Pitch, TargetPitchAngle, DeltaSeconds, InterpSpeed);

		if (FMath::IsNearlyZero(NewPitch, 0.01f) && FMath::IsNearlyZero(UserPitch, 0.01f))
		{
			NewPitch = 0.f;
		}
		CurrentRot.Pitch = NewPitch;
		Controlled->Airframe->SetRelativeRotation(CurrentRot);
	}
}

void UFlightComponent::ApplyYaw(float DeltaSeconds)
{
	float InterpSpeed = 0;
	if (Controlled->GetController() && Controlled->GetController()->IsPlayerController())
	{
		InterpSpeed = AircraftStats->RudderRate * 50;
		float TargetYawRate = UserYaw * InterpSpeed;
		NextYaw = FMath::FInterpTo(UserYaw, TargetYawRate, DeltaSeconds, 5.f);

		FRotator DeltaRot(0.f, NextYaw * DeltaSeconds,0.f);
		Controlled->Airframe->AddLocalRotation(DeltaRot);
	}
	else
	{
		InterpSpeed = AircraftStats->RudderRate;
		float TargetYawAngle = UserYaw * InterpSpeed;
		FRotator CurrentRot = Controlled->Airframe->GetRelativeRotation();
		float NewYaw = FMath::FInterpTo(CurrentRot.Yaw, TargetYawAngle, DeltaSeconds, InterpSpeed);

		if (FMath::IsNearlyZero(NewYaw, 0.01f) && FMath::IsNearlyZero(UserYaw, 0.01f))
		{
			NewYaw = 0.f;
		}
		CurrentRot.Yaw = NewYaw;
		Controlled->Airframe->SetRelativeRotation(CurrentRot);
	}
}

void UFlightComponent::ApplyRoll(float DeltaSeconds)
{
	float InterpSpeed = 0;
	if (Controlled->GetController() && Controlled->GetController()->IsPlayerController())
	{
		 InterpSpeed = AircraftStats->RollRate * 10;
		 float TargetRollRate = UserRoll * InterpSpeed;
		 NextRoll = FMath::FInterpTo(NextRoll, TargetRollRate, DeltaSeconds, 5.f);

		 FRotator DeltaRot(0.f, 0.f, NextRoll * DeltaSeconds);
		 Controlled->Airframe->AddLocalRotation(DeltaRot);
	}
	else 
	{
		 InterpSpeed = AircraftStats->RollRate;
		 float TargetRollAngle = UserRoll * InterpSpeed;
		 FRotator CurrentRot = Controlled->Airframe->GetRelativeRotation();
		 float NewRoll = FMath::FInterpTo(CurrentRot.Roll, TargetRollAngle, DeltaSeconds, InterpSpeed);

		 if (FMath::IsNearlyZero(NewRoll, 0.01f) && FMath::IsNearlyZero(UserRoll, 0.01f))
			 NewRoll = 0.f;

		 CurrentRot.Roll = NewRoll;
		 Controlled->Airframe->SetRelativeRotation(CurrentRot);
	}
}

void UFlightComponent::SetPitch(float PitchValue) {UserPitch = PitchValue;}

void UFlightComponent::SetYaw(float YawValue) {UserYaw = YawValue;}

void UFlightComponent::SetRoll(float RollValue) {UserRoll = RollValue;}

void UFlightComponent::SetThrust(float Thrust) {CurrentThrust = Thrust;}