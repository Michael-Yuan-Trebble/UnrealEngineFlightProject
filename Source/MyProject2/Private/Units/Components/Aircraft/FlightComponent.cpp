// Fill out your copyright notice in the Description page of Project Settings.

#include "Units/Components/Aircraft/FlightComponent.h"
#include "Units/Aircraft/BaseAircraft.h"
#include "Components/BoxComponent.h"
#include "Debug/DebugHelper.h"

EThrottleStage UFlightComponent::getThrottleStage(const float throttle)
{
	if (throttle >= 0.f && throttle <= 0.4f) return EThrottleStage::Slow;
	else if (throttle <= 0.8f) return EThrottleStage::Normal;
	else return EThrottleStage::Afterburner;
}

UFlightComponent::UFlightComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UFlightComponent::Setup(ABaseAircraft* InControl, const UAircraftStats* InStats)
{
	Controlled = InControl;
	if (!IsValid(Controlled)) return;
	Airframe = Controlled->GetAirframe();
	AirStats = InStats->InGameAirStats;
}

void UFlightComponent::SetFlightMode(const EFlightMode InFlight) 
{
	FlightMode = InFlight;
	switch (InFlight) 
	{
	case EFlightMode::Flight:
		FlightDrag = 0.f; 
		isFlying = true;
		break;
	case EFlightMode::Gears:
		FlightDrag = AirStats.Acceleration * 0.5f;
		break;
	case EFlightMode::Naval:
		bLanded = true;
		break;
	default:
		FlightDrag = 0.f;
		break;
	}
}

void UFlightComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ====================================
	// Movement Application Components
	// ====================================

	if (FlightMode != EFlightMode::Flight) CheckLanding(DeltaTime);

	ApplySpeed(CurrentThrust, DeltaTime);

	if (FlightMode == EFlightMode::Naval && bLanded) return;

	RollAOA(DeltaTime);
	ApplyRot(DeltaTime);

	if (currentKMHSpeed <= DropSpeed)
	{
		if (!bDropping) bDropping = true;
		AddDropSpeed(DeltaTime);
		if (bLanded)
		{
			Landed(DeltaTime);
			bCanTakeOff = false;
		}
		else if (currentKMHSpeed <= StallSpeed)
		{
			// TODO: Stall should enter a state where it goes down, but also skips that drop speed process because it feels clunky
			Stall(DeltaTime);
		}
	}
	else if (bDropping) bDropping = false;

	if (!bCanTakeOff && currentKMHSpeed > DropSpeed)
	{
		bCanTakeOff = true;
		bLanded = false;
	}

	if (!bDropping && !bLanded) ReturnAOA(DeltaTime);
}

void UFlightComponent::ApplySpeed(const float ThrottlePercentage, const float DeltaSeconds)
{
	currentStage = getThrottleStage(ThrottlePercentage);
	switch (currentStage)
	{
		case EThrottleStage::Slow: SlowSpeed(ThrottlePercentage); break;
		case EThrottleStage::Normal: NormalSpeed(ThrottlePercentage); break;
		case EThrottleStage::Afterburner: AfterburnerSpeed(ThrottlePercentage);break;
	}

	if (prevStage != currentStage)
	{
		switchingPhase = true;
		if (prevStage == EThrottleStage::Afterburner) OnAfterburnerEngaged.Broadcast(false);
		else if (currentStage == EThrottleStage::Afterburner) OnAfterburnerEngaged.Broadcast(true);
	}

	prevStage = currentStage;

	if (FlightMode == EFlightMode::Naval && bRestrained) return;

	float speedDrag = CalculateSpeedDrag();
	float AOADrag = 0.f;
	if (!bDropping) AOADrag = FMath::IsFinite(DragAOA()) ? DragAOA() : 0.f;
	float pitchDrag = FMath::IsFinite(PitchDrag()) ? PitchDrag() : 0.f;

	float totalDrag = speedDrag + AOADrag + pitchDrag;

	float trueAcceleration = Acceleration - totalDrag - FlightDrag;
	trueAcceleration = FMath::Abs(trueAcceleration) < 0.001f ? 0.f : trueAcceleration;

	// Convert to meters
	currentSpeed += (trueAcceleration / 0.01f) * DeltaSeconds;

	if (!FMath::IsFinite(currentSpeed)) currentSpeed = 0.f;
	currentSpeed = FMath::Clamp(currentSpeed, 0.f, 1000000.f);
	currentKMHSpeed = UFlightMathLibrary::SpeedToKMH(currentSpeed);

	PreviousVelocity = Velocity;
	if (bLanded && IsValid(Airframe)) Velocity = Airframe->GetForwardVector() * currentSpeed;
	else if (IsValid(Controlled)) Velocity = Controlled->GetActorForwardVector() * currentSpeed;

	if (Velocity.ContainsNaN() || Velocity.Size() > 1e6f) Velocity = FVector::ZeroVector;

	if (bLanded && !bCanTakeOff) 
		Velocity.Z = 0.f;

	if (IsValid(Controlled)) Controlled->AddActorWorldOffset(Velocity * DeltaSeconds, true);
	CalculateGForce(DeltaSeconds);
}

void UFlightComponent::SetInitialSpeed(const float Speed)
{
	currentSpeed = Speed;
	if (IsValid(Controlled)) Velocity = Controlled->GetActorForwardVector() * Speed;
}

void UFlightComponent::AddSpeed(const float Speed, const float D) {
	currentSpeed += UFlightMathLibrary::ConvertKMHToSpeed(Speed) + ((Acceleration / 0.01f) * D);
}

void UFlightComponent::CheckLanding(const float D)
{
	if (!IsValid(Controlled) || !IsValid(Controlled->GetLandingGearCollision())) return;
	if (bLanded && !bCanTakeOff) return;
	FHitResult Hit;
	FVector Start = Controlled->GetLandingGearCollision()->GetComponentLocation();
	FVector End = Start - Controlled->GetActorUpVector() * 110.f;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Controlled);
	FCollisionObjectQueryParams ObjParams;
	ObjParams.AddObjectTypesToQuery(ECC_WorldStatic);
	bLanded = GetWorld()->LineTraceSingleByObjectType(Hit, Start, End, ObjParams, Params);
}

void UFlightComponent::AddDropSpeed(const float D) {

	// It doesn't have to go to 90! it can go to something like 30 or 40, once stalling then push it to 90 for the user to gain speed then exit stall
	if (currentKMHSpeed <= StallSpeed) return;

	float MaxDropAngle = 30.f;

	float Percent = FMath::Clamp(currentKMHSpeed / DropSpeed, 0.f, 1.f);
	
	float CurrentMaxAngleDrop = MaxDropAngle * (1 - Percent);

	TempRecovery(D, CurrentMaxAngleDrop);
}

void UFlightComponent::Stall(const float D)
{
	if (!IsValid(Airframe)) return;
	FQuat AirframeCurrentRelQuat = Airframe->GetRelativeRotation().Quaternion();
	FQuat IdentityQuat = FQuat::Identity;
	float NoseAlpha = FMath::Clamp(AirStats.AOARecoverySpeed * D, 0.f, 1.f);
	FQuat NewRelQuat = FQuat::Slerp(AirframeCurrentRelQuat, IdentityQuat, NoseAlpha);
	Airframe->SetRelativeRotation(NewRelQuat.Rotator());
}

void UFlightComponent::Landed(const float D)
{
	if (!IsValid(Airframe)) return;
	FRotator Rot = Airframe->GetComponentRotation();
	if (FMath::Abs(Rot.Pitch) > 0.f || !bCanTakeOff) 
	{
		Rot.Pitch = FMath::FInterpTo(Rot.Pitch, 0.f, D, 4.f);
		Airframe->SetWorldRotation(Rot);
	}
	if (FMath::Abs(Rot.Roll) > 0.f) 
	{
		Rot.Roll = FMath::FInterpTo(Rot.Roll, 0.f, D, 4.f);
		Airframe->SetWorldRotation(Rot);
	}
}

float UFlightComponent::CalculateSpeedDrag() 
{
	float drag = 0.f;
	float totalFlightPercent = (targetSpeed != 0.f) ? (currentSpeed / targetSpeed) : 0.f;

	if (totalFlightPercent >= 0.8f && totalFlightPercent <= 1.05f)
	{
		drag = Acceleration / (1.05f + FMath::Pow(2.f, -0.01f * (currentSpeed - targetSpeed)));
	}
	else if (totalFlightPercent > 1.05f && switchingPhase)
	{
		drag = Acceleration * 4.f / (1.f + FMath::Pow(2.f, -0.01f * (currentSpeed - targetSpeed)));
	}
	else if (switchingPhase) switchingPhase = false;

	return FMath::Clamp(drag, 0.f, Acceleration * 10.f);
}

void UFlightComponent::CalculateGForce(const float DeltaSeconds)
{
	FVector vectorAccel = (Velocity - PreviousVelocity) / DeltaSeconds / 100.f;

	float alpha = 0.15f;
	FVector accel = FMath::Lerp(accel, vectorAccel, alpha);

	if (!IsValid(Airframe)) return;
	FVector LocalAccel = Airframe->GetComponentTransform().InverseTransformVector(vectorAccel);
	FVector Gs = LocalAccel / 9.81f;

	// Starts at 0 G, so just adding one because of gravity
	float VerticalG = Gs.Z + 1.f;
	previousGForce = displayG;

	// Vortex

	displayG = FMath::RoundToInt(VerticalG);
	CalculateVortex();
}

void UFlightComponent::CalculateVortex() 
{
	if (displayG >= VortexG && previousGForce < VortexG) OnVortexActivate.Broadcast(true);
	else if (displayG < VortexG && previousGForce >= VortexG) OnVortexActivate.Broadcast(false);
}

// ====================================
// Throttle Stage for Acceleration and Target Speed
// ====================================

void UFlightComponent::SlowSpeed(const float ThrottlePercentage)
{
	float negation = 1.f - ThrottlePercentage;
	targetSpeed = 0.f;

	float scale = (ThrottlePercentage <= 0.1f) ? 3.f : 1.25f;
	Acceleration = -(AirStats.Acceleration * negation * scale);
}

void UFlightComponent::NormalSpeed(const float ThrottlePercentage)
{
	Acceleration = AirStats.Acceleration * ThrottlePercentage;
	targetSpeed = AirStats.MaxSpeed * 0.5f;
}

void UFlightComponent::AfterburnerSpeed(const float ThrottlePercentage)
{
	if (ThrottlePercentage >= 0.9f)
	{
		Acceleration = AirStats.Acceleration * ThrottlePercentage * 2.f;
		targetSpeed = AirStats.MaxSpeed;
		return;
	}
	Acceleration = AirStats.Acceleration * ThrottlePercentage * 1.25f;
	targetSpeed = AirStats.MaxSpeed * 0.9f;
}

// ====================================
// Turning/Rotation Calculations
// ====================================

void UFlightComponent::ApplyRot(const float DeltaSeconds)
{
	if (FlightMode == EFlightMode::Naval && bLanded) return;

	ApplyYaw(DeltaSeconds);

	if (bLanded && bCanTakeOff) 
	{
		ApplyPitch(DeltaSeconds);
	}
	else if (!bLanded)
	{
		ApplyRoll(DeltaSeconds);
		ApplyPitch(DeltaSeconds);
	}
}

void UFlightComponent::ReturnAOA(const float DeltaSeconds)
{
	// Rotates the Root vector toward Airframe's vector
	if (!IsValid(Controlled) || !IsValid(Airframe)) return;
	FQuat CurrentQuat = Controlled->GetActorQuat();
	FQuat TargetQuat = Airframe->GetComponentQuat();
	float RootTurnSpeed = 2.0f;
	float RootAlpha = FMath::Clamp(RootTurnSpeed * DeltaSeconds, 0.f, 1.f);
	FQuat NewRootQuat = FQuat::Slerp(CurrentQuat, TargetQuat, RootAlpha);

	if (IsValid(Controlled)) Controlled->SetActorRotation(NewRootQuat);

	// Resets the Airframe's vector 
	FQuat AirframeCurrentRelQuat = Airframe->GetRelativeRotation().Quaternion();
	FQuat IdentityQuat = FQuat::Identity;
	float NoseAlpha = FMath::Clamp(AirStats.AOARecoverySpeed * DeltaSeconds, 0.f, 1.f);
	FQuat NewRelQuat = FQuat::Slerp(AirframeCurrentRelQuat, IdentityQuat, NoseAlpha);
	if (IsValid(Airframe)) Airframe->SetRelativeRotation(NewRelQuat.Rotator());
}

void UFlightComponent::TempRecovery(const float D, const float Deg)
{
	if (!IsValid(Controlled) || !IsValid(Airframe)) return;

	FQuat CurrentRootQuat = Controlled->GetActorQuat();
	FQuat AirframeQuat = Airframe->GetComponentQuat();

	FVector WorldForward = FVector::ForwardVector;
	FVector PitchAxis = FVector::CrossProduct(WorldForward, FVector::UpVector).GetSafeNormal();
	FQuat DownOffset = FQuat(PitchAxis, FMath::DegreesToRadians(-Deg));

	FQuat TargetRootQuat = AirframeQuat * DownOffset;

	float RootAlpha = FMath::Clamp(2.0f * D, 0.f, 1.f);
	FQuat NewRootQuat = FQuat::Slerp(CurrentRootQuat, TargetRootQuat, RootAlpha);
	if (IsValid(Controlled)) Controlled->SetActorRotation(NewRootQuat);

	FQuat AirframeCurrentRelQuat = Airframe->GetRelativeRotation().Quaternion();

	FQuat DesiredRelQuat = DownOffset.Inverse();

	float NoseAlpha = FMath::Clamp(AirStats.AOARecoverySpeed * D, 0.f, 1.f);
	FQuat NewAirframeRelQuat = FQuat::Slerp(AirframeCurrentRelQuat, DesiredRelQuat, NoseAlpha);

	if (IsValid(Airframe)) Airframe->SetRelativeRotation(NewAirframeRelQuat);
}

float UFlightComponent::GetAOA() 
{
	if (!IsValid(Airframe)) return 0.f;
	FVector Forward = Airframe->GetForwardVector().GetSafeNormal();
	FVector VelocityDir = Velocity.GetSafeNormal();
	if (Velocity.IsNearlyZero()) VelocityDir = Forward;
	float CosTheta = FVector::DotProduct(Forward, VelocityDir);
	CosTheta = FMath::Clamp(CosTheta, -1.f, 1.f);
	float AOA = FMath::RadiansToDegrees(FMath::Acos(CosTheta));

	// TODO: Might integrate this part later, used to determine +- of AOA
	// FVector Cross = FVector::CrossProduct(Forward, VelocityDir);
	//float Sign = (Cross.Z >= 0.f) ? 1.f : -1.f;
	//AOA *= Sign;

	AOA = FMath::Abs(AOA);
	float AOARadians = FMath::DegreesToRadians(AOA);
	return AOARadians;
}

float UFlightComponent::DragAOA()
{
	float dragCo = AirStats.DragCoefficient;
	float drag = FMath::Abs(GetAOA()) * dragCo * (currentKMHSpeed / 4.f);
	return drag;
}

void UFlightComponent::RollAOA(const float DeltaSeconds)
{
	// ====================================
	// Apply a downward AOA based on user's roll orientation with the ground
	// ====================================

	// TODO: Make this do it on root not airframe

	if (!IsValid(Airframe)) return;

	FVector Forward = Airframe->GetForwardVector();
	FVector Up = Airframe->GetUpVector();

	float PitchAngle = FMath::RadiansToDegrees(
		FMath::Atan2(Forward.Z, FVector(Forward.X, Forward.Y, 0.f).Size())
	);

	float UpDot = FVector::DotProduct(Up, FVector::UpVector);
	float DownPitchStrength = FMath::Clamp(1.f - UpDot, 0.f, 2.f) * 2.f;
	float DownPitch = -DownPitchStrength * DeltaSeconds;

	FVector WorldPitchAxis = FVector::CrossProduct(Forward, FVector::UpVector).GetSafeNormal();

	// Prevent potential Gimbal Lock, investigating further if it is truly needed
	if (PitchAngle > -85.f)
	{
		FQuat Rotate = FQuat(WorldPitchAxis, FMath::DegreesToRadians(DownPitch));
		Airframe->AddWorldRotation(Rotate);
	}
}

float UFlightComponent::PitchDrag() 
{
	if (!IsValid(Airframe)) return 0.f;
	FVector Up = Airframe->GetUpVector();
	FVector Forward = Airframe->GetForwardVector();

	float PitchAngle = FMath::RadiansToDegrees(
		FMath::Atan2(Forward.Z, FVector(Forward.X, Forward.Y, 0.f).Size())
	);

	float RadAngle = FMath::DegreesToRadians(PitchAngle);
	float UpDot = FVector::DotProduct(Up, FVector::UpVector);

	// TODO: Make this more robust
	return PitchAngle / 50.f;
}

// ====================================
// Apply Rotations
// ====================================

void UFlightComponent::ApplyPitch(const float DeltaSeconds)
{
	if (bLanded && !bCanTakeOff) return;
	if (!IsValid(AirStats.DragCurve) || !IsValid(Airframe)) return;
	float CurveTurn = AirStats.DragCurve->GetFloatValue(currentKMHSpeed);
	float CompressionTurn = AirStats.CompressionCurve->GetFloatValue(currentKMHSpeed);
	if (UserRotation.Pitch == 0.f)
	{
		NextRotation.Pitch = FMath::FInterpTo(NextRotation.Pitch, 0.f, DeltaSeconds, 3.f);
		FRotator DeltaRot(NextRotation.Pitch * DeltaSeconds, 0.f, 0.f);
		Airframe->AddLocalRotation(DeltaRot);
		return;
	}
		
	float InterpSpeed = AirStats.TurnRate * CurveTurn * CompressionTurn;
	InterpSpeed = UserRotation.Pitch < 0.f ? InterpSpeed * 0.5f : InterpSpeed;

	float TargetPitchRate = UserRotation.Pitch * InterpSpeed;
	NextRotation.Pitch = FMath::FInterpTo(NextRotation.Pitch, TargetPitchRate, DeltaSeconds, 5.f);

	FRotator DeltaRot(NextRotation.Pitch * DeltaSeconds, 0.f, 0.f);
	Airframe->AddLocalRotation(DeltaRot);
}

void UFlightComponent::ApplyYaw(const float DeltaSeconds)
{
	if (!IsValid(Airframe)) return;
	if (UserRotation.Yaw == 0.f) {
		NextRotation.Yaw = FMath::FInterpTo(NextRotation.Yaw, 0.f, DeltaSeconds, 3.f);
		FRotator DeltaRot(0.f, NextRotation.Yaw * DeltaSeconds, 0.f);
		Airframe->AddLocalRotation(DeltaRot);
		return;
	}
	float TargetYawRate = UserRotation.Yaw * AirStats.RudderRate;
	NextRotation.Yaw = FMath::FInterpTo(NextRotation.Yaw, TargetYawRate, DeltaSeconds, 2.f);
	FRotator DeltaRot(0.f, NextRotation.Yaw * DeltaSeconds, 0.f);
	Airframe->AddLocalRotation(DeltaRot);
}

void UFlightComponent::ApplyRoll(const float DeltaSeconds)
{
	if (!IsValid(Airframe)) return;
	if (UserRotation.Roll == 0.f)
	{
		NextRotation.Roll = FMath::FInterpTo(NextRotation.Roll, 0.f, DeltaSeconds, 3.f);
		FRotator DeltaRot(0.f, 0.f, NextRotation.Roll * DeltaSeconds);
		Airframe->AddLocalRotation(DeltaRot);
		return;
	}
	float TargetRollRate = UserRotation.Roll * AirStats.RollRate;
	NextRotation.Roll = FMath::FInterpTo(NextRotation.Roll, TargetRollRate, DeltaSeconds, 5.f);
	FRotator DeltaRot(0.f, 0.f, NextRotation.Roll * DeltaSeconds);
	Airframe->AddLocalRotation(DeltaRot);
}