// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Flight Component!"));
#include "Aircraft/FlightComponent.h"

EThrottleStage getThrottleStage(float throttle)
{
	// Throttle Stages for pronounced acceleration differences
	throttle *= 100.f;
	if (throttle >= 0 && throttle <= 40) return EThrottleStage::Slow;
	else if (throttle <= 80) return EThrottleStage::Normal;
	else return EThrottleStage::Afterburner;
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
	if (!Controlled || !AircraftStats) return;
	if (!isFlying) isFlying = currentSpeed > 10.f;

	// ====================================
	// Movement Application Components
	// ====================================

	ApplySpeed(CurrentThrust, DeltaTime);
	RollAOA(DeltaTime);
	ApplyRot(DeltaTime);

	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UFlightComponent::ApplySpeed(float ThrottlePercentage, float DeltaSeconds)
{
	EThrottleStage currentStage = getThrottleStage(ThrottlePercentage);
	switch (currentStage)
	{
		case EThrottleStage::Slow: SlowSpeed(ThrottlePercentage); break;
		case EThrottleStage::Normal: NormalSpeed(ThrottlePercentage); break;
		case EThrottleStage::Afterburner: AfterburnerSpeed(ThrottlePercentage);break;
	}

	if (prevStage != currentStage) switchingPhase = true;

	AdjustSpringArm(DeltaSeconds, ThrottlePercentage);

	float drag = 0;
	float trueAcceleration = Acceleration;

	// Target speed for better acceleration until a certain point feeling

	float totalFlightPercent = (targetSpeed != 0) ? (currentSpeed / targetSpeed) : 0.f;

	if (totalFlightPercent >= 0.8 && totalFlightPercent <= 1.05)
	{
		drag = Acceleration / (1.05f + FMath::Pow(2.f, -0.01f * (currentSpeed - targetSpeed)));
	} 
	else if (totalFlightPercent > 1.05 && switchingPhase) 
	{
		drag = Acceleration * 4 / (1.f + FMath::Pow(2.f, -0.01f * (currentSpeed - targetSpeed)));
	}
	else if (switchingPhase)
	{
		switchingPhase = false;
	}

	prevStage = currentStage;

	// Safety check
	drag = FMath::Clamp(drag, 0, Acceleration * 10.f);
	trueAcceleration -= drag;

	float t = DragAOA();
	if (!FMath::IsFinite(t)) t = 0;

	float pitchDrag = PitchDrag();
	if (!FMath::IsFinite(pitchDrag)) pitchDrag = 0;

	trueAcceleration -= t + pitchDrag;
	trueAcceleration = FMath::Abs(trueAcceleration) < 0.01 ? 0 : trueAcceleration;

	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Pitch: %.2f"), drag));

	currentSpeed += trueAcceleration;
	currentSpeed = FMath::FInterpTo(currentSpeed, Velocity.Size(), DeltaSeconds, 5.f);

	if (!FMath::IsFinite(currentSpeed) || FMath::IsNaN(currentSpeed)) currentSpeed = 0;

	currentSpeed = FMath::Clamp(currentSpeed, 0, 1000000);

	PreviousVelocity = Velocity;

	Velocity = Controlled->GetActorForwardVector() * currentSpeed;

	if (Velocity.ContainsNaN() || !FMath::IsFinite(Velocity.X) || Velocity.Size() > 1e6f) return;

	Controlled->AddActorWorldOffset(Velocity * DeltaSeconds, true);

	FVector vectorAccel = (Velocity - PreviousVelocity) / DeltaSeconds;
	
	vectorAccel += FVector(0.f, 0.f, -9.81f);

	FVector LocalAccel = Controlled->Airframe->GetComponentTransform().InverseTransformVector(vectorAccel);
	FVector Gs = LocalAccel / 9.81f;

	float TotalG = Gs.Size();

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Pitch: %.2f"), TotalG));

	// ====================================
	// Draw AOA lines
	// Blue = Root forward
	// Red = Mesh Forward
	// ====================================

	DrawDebugLine(GetWorld(), Controlled->GetActorLocation(), Controlled->GetActorLocation() + Controlled->GetActorForwardVector() * 300, FColor::Blue, false, 0.f, 0, 2.f);
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
		Acceleration = -(AircraftStats->Acceleration * negation * 5);
		targetSpeed = 0;
		return;
	}
	Acceleration = -(AircraftStats->Acceleration * negation * 2);
	targetSpeed = 0;
}

void UFlightComponent::NormalSpeed(float ThrottlePercentage)
{
	Acceleration = AircraftStats->Acceleration * ThrottlePercentage;
	targetSpeed = AircraftStats->MaxSpeed * 0.5;
}

void UFlightComponent::AfterburnerSpeed(float ThrottlePercentage)
{
	if (ThrottlePercentage >= 0.9)
	{
		Acceleration = AircraftStats->Acceleration * ThrottlePercentage * 5;
		targetSpeed = AircraftStats->MaxSpeed;
		return;
	}
	Acceleration = AircraftStats->Acceleration * ThrottlePercentage * 2;
	targetSpeed = AircraftStats->MaxSpeed * 0.9;
}

// ====================================
// Turning/Rotation Calculations
// ====================================

void UFlightComponent::ApplyRot(float DeltaSeconds)
{
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
	//TODO: Move SpringArm based on User's throttle input
	//Controlled->SpringArm->TargetArmLength = FMath::FInterpTo(Controlled->SpringArm->TargetArmLength, AircraftStats->SpringArmLength + (50 * (0.5 - ThrottlePercentage)), DeltaSeconds, 2.f);
}

float UFlightComponent::DragAOA()
{
	FVector F = Controlled->Airframe->GetForwardVector().GetSafeNormal();
	FVector VelocityDir = Velocity.GetSafeNormal();
	if (Velocity.IsNearlyZero()) VelocityDir = F;
	float CosTheta = FVector::DotProduct(F, VelocityDir);
	CosTheta = FMath::Clamp(CosTheta, -1.0f, 1.0f);
	float AOA = FMath::RadiansToDegrees(FMath::Acos(CosTheta));
	FVector Cross = FVector::CrossProduct(F, VelocityDir);
	float Sign = (Cross.Z >= 0.f) ? 1.f : -1.f;
	AOA *= Sign;

	AOA = FMath::Abs(AOA);
	float AOARadians = FMath::DegreesToRadians(AOA);
	float drag = 15 * FMath::Pow(AOARadians, 2);
	//if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("%f"), drag));
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

float UFlightComponent::PitchDrag() 
{
	FVector Up = Controlled->Airframe->GetUpVector();
	FVector Forward = Controlled->Airframe->GetForwardVector();

	float PitchAngle = FMath::RadiansToDegrees(
		FMath::Atan2(Forward.Z, FVector(Forward.X, Forward.Y, 0.f).Size())
	);

	float UpDot = FVector::DotProduct(Up, FVector::UpVector);

	return PitchAngle / 50;

	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Pitch: %.2f"), PitchAngle));
	
}

// ====================================
// Apply Rotations
// ====================================

void UFlightComponent::ApplyPitch(float DeltaSeconds)
{
	float InterpSpeed = 0;
	if (Controlled->GetController() && Controlled->GetController()->IsPlayerController())
	{
		if (UserPitch == 0)
		{
			NextPitch = FMath::FInterpTo(NextPitch, 0, DeltaSeconds, 3.f);
			FRotator DeltaRot(NextPitch * DeltaSeconds, 0.f, 0.f);
			Controlled->Airframe->AddLocalRotation(DeltaRot);
			return;
		}

		InterpSpeed = AircraftStats->TurnRate * 50;
		if (UserPitch < 0) 
		{
			InterpSpeed = AircraftStats->TurnRate * 20;
		}
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
		if (UserYaw == 0) {
			NextYaw = FMath::FInterpTo(NextYaw, 0, DeltaSeconds, 3.f);
			FRotator DeltaRot(0.f, NextYaw * DeltaSeconds,0.f);
			Controlled->Airframe->AddLocalRotation(DeltaRot);
			return;
		}
		InterpSpeed = AircraftStats->RudderRate * 50;
		float TargetYawRate = UserYaw * InterpSpeed;
		NextYaw = FMath::FInterpTo(UserYaw, TargetYawRate, DeltaSeconds, 2.f);

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
		 if (UserRoll == 0) 
		 {
			 NextRoll = FMath::FInterpTo(NextRoll, 0, DeltaSeconds, 3.f);
			 FRotator DeltaRot(0.f, 0.f, NextRoll * DeltaSeconds);
			 Controlled->Airframe->AddLocalRotation(DeltaRot);
			 return;
		 }
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