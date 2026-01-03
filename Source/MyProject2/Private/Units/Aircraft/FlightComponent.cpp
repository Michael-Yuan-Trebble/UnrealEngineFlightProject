// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Flight Component!"));
#include "Units/Aircraft/FlightComponent.h"
#include "Units/Aircraft/Player/PlayerAircraft.h"

EThrottleStage UFlightComponent::getThrottleStage(float throttle)
{
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

void UFlightComponent::SetFlightMode(EFlightMode InFlight) 
{
	FlightMode = InFlight;
	switch (InFlight) 
	{
	case EFlightMode::Flight:
		FlightDrag = 0.f; 
		isFlying = true;
		break;
	case EFlightMode::Gears:
		if (IsValid(AircraftStats)) FlightDrag = AircraftStats->Acceleration * 0.5;
		break;
	case EFlightMode::Naval:
		if (IsValid(AircraftStats)) FlightDrag = AircraftStats->Acceleration * 0.5;
		bRestrained = true;
		break;
	default:
		FlightDrag = 0.f;
		break;
	}
}

void UFlightComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (!IsValid(Controlled) || !IsValid(Controlled->Airframe) || !AircraftStats) return;

	// ====================================
	// Movement Application Components
	// ====================================

	if (FlightMode != EFlightMode::Flight) CheckLanding(DeltaTime);

	ApplySpeed(CurrentThrust, DeltaTime);
	RollAOA(DeltaTime);
	ApplyRot(DeltaTime);

	if (GetCurrentSpeedKMH() <= DropSpeed) 
	{
		if (!bDropping) bDropping = true;
		AddDropSpeed(DeltaTime);
		if (bLanded)
		{
			Landed(DeltaTime);
			bCanTakeOff = false;
		}
		else if (GetCurrentSpeedKMH() <= StallSpeed) 
		{
			Stall(DeltaTime);
		}
	}
	else if (bDropping) bDropping = false;

	if (!bCanTakeOff && GetCurrentSpeedKMH() > DropSpeed)
	{
		bCanTakeOff = true;
		bLanded = false;
	}

	if (!bDropping && !bLanded) ReturnAOA(DeltaTime);
}

void UFlightComponent::ApplySpeed(float ThrottlePercentage, float DeltaSeconds)
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

	float speedDrag = CalculateSpeedDrag();
	float AOADrag = 0;
	if (!bDropping) AOADrag = FMath::IsFinite(DragAOA()) ? DragAOA() : 0;
	float pitchDrag = FMath::IsFinite(PitchDrag()) ? PitchDrag() : 0;

	float totalDrag = speedDrag + AOADrag + pitchDrag;

	float trueAcceleration = Acceleration - totalDrag - FlightDrag;
	trueAcceleration = FMath::Abs(trueAcceleration) < 0.001 ? 0 : trueAcceleration;

	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Pitch: %.2f"), drag));

	currentSpeed += trueAcceleration;
	currentSpeed = FMath::FInterpTo(currentSpeed, Velocity.Size(), DeltaSeconds, 5.f);

	if (!FMath::IsFinite(currentSpeed) || currentSpeed < 0.f) currentSpeed = 0;
	currentSpeed = FMath::Clamp(currentSpeed, 0, 1000000);

	PreviousVelocity = Velocity;
	if (bLanded && IsValid(Controlled->Airframe)) Velocity = Controlled->Airframe->GetForwardVector() * currentSpeed;
	else Velocity = Controlled->GetActorForwardVector() * currentSpeed;

	if (Velocity.ContainsNaN() || Velocity.Size() > 1e6f) Velocity = FVector::ZeroVector;

	if (!IsValid(Controlled)) return;

	if (bLanded && !bCanTakeOff) { 
		Velocity.Z = 0; 
	}

	Controlled->AddActorWorldOffset(Velocity * DeltaSeconds, true);
	CalculateGForce(DeltaSeconds);
}

void UFlightComponent::CheckLanding(float D) 
{
	if (!IsValid(Controlled->LandingGearCollision)) return;
	if (bLanded && !bCanTakeOff) return;
	FHitResult Hit;
	FVector Start = Controlled->LandingGearCollision->GetComponentLocation();
	FVector End = Start - Controlled->GetActorUpVector() * 110;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Controlled);
	FCollisionObjectQueryParams ObjParams;
	ObjParams.AddObjectTypesToQuery(ECC_WorldStatic);
	bLanded = GetWorld()->LineTraceSingleByObjectType(Hit, Start, End, ObjParams, Params);
}

void UFlightComponent::AddDropSpeed(float D) {
	if (!IsValid(Controlled) || !IsValid(Controlled->Airframe)) return;

	float Speed = GetCurrentSpeedKMH();
	float Percent = FMath::Clamp(Speed / DropSpeed, 0.f, 1.f);

	FRotator AirframeRot = Controlled->Airframe->GetComponentRotation();

	FRotator NewRot = Controlled->GetActorRotation();

	float t = (Percent * 180.f) - 90.f;

	if (NewRot.Pitch > t) 
	{
		if (t == -90) t = -89.999f;
		if (t == 90) t = 89.999f;
		NewRot.Pitch = FMath::FInterpTo(NewRot.Pitch,t,D,5);
	}
	else 
	{
		// Maybe use apply pitch instead of this?
		NewRot.Pitch = FMath::FInterpTo(NewRot.Pitch, AirframeRot.Pitch,D,5);
	}

	GEngine->AddOnScreenDebugMessage(-1, 0.01f, FColor::Yellow, FString::Printf(TEXT("Forward Pitch Deg: %.2f"), Percent));

	NewRot.Yaw = FMath::FInterpTo(NewRot.Yaw, AirframeRot.Yaw, D, 5.f);

	Controlled->SetActorRotation(NewRot);
	Controlled->Airframe->SetWorldRotation(AirframeRot);
}

void UFlightComponent::Stall(float D) 
{
	if (!IsValid(Controlled) || !IsValid(Controlled->Airframe)) return;
	FQuat AirframeCurrentRelQuat = Controlled->Airframe->GetComponentQuat();
	FQuat TargetQuat = Controlled->GetActorRotation().Quaternion();
	float NoseAlpha = FMath::Clamp(1 * D, 0.f, 1.f);
	FQuat NewRelQuat = FQuat::Slerp(AirframeCurrentRelQuat, TargetQuat, NoseAlpha);
	if (IsValid(Controlled) && IsValid(Controlled->Airframe)) Controlled->Airframe->SetWorldRotation(NewRelQuat.Rotator());
}

void UFlightComponent::Landed(float D) 
{
	if (!IsValid(Controlled) || !IsValid(Controlled->Airframe)) return;
	FRotator Rot = Controlled->Airframe->GetComponentRotation();
	if (FMath::Abs(Rot.Pitch) > 0.f || !bCanTakeOff) 
	{
		Rot.Pitch = FMath::FInterpTo(Rot.Pitch, 0.f, D, 4.f);
		Controlled->Airframe->SetWorldRotation(Rot);
	}
	if (FMath::Abs(Rot.Roll) > 0.f) 
	{
		Rot.Roll = FMath::FInterpTo(Rot.Roll, 0.f, D, 4.f);
		Controlled->Airframe->SetWorldRotation(Rot);
	}
}

float UFlightComponent::CalculateSpeedDrag() 
{
	float drag = 0;
	float totalFlightPercent = (targetSpeed != 0) ? (currentSpeed / targetSpeed) : 0.f;

	if (totalFlightPercent >= 0.8 && totalFlightPercent <= 1.05)
	{
		drag = Acceleration / (1.05f + FMath::Pow(2.f, -0.01f * (currentSpeed - targetSpeed)));
	}
	else if (totalFlightPercent > 1.05 && switchingPhase)
	{
		drag = Acceleration * 4 / (1.f + FMath::Pow(2.f, -0.01f * (currentSpeed - targetSpeed)));
	}
	else if (switchingPhase) switchingPhase = false;

	return FMath::Clamp(drag, 0, Acceleration * 10.f);
}

void UFlightComponent::CalculateGForce(float DeltaSeconds) 
{
	FVector vectorAccel = (Velocity - PreviousVelocity) / DeltaSeconds / 100.f;

	float alpha = 0.15f;
	FVector accel = FMath::Lerp(accel, vectorAccel, alpha);

	FVector LocalAccel = Controlled->Airframe->GetComponentTransform().InverseTransformVector(vectorAccel);
	FVector Gs = LocalAccel / 9.81f;

	// Starts at 0 G, so just adding one because of gravity
	float VerticalG = Gs.Z + 1;
	previousGForce = displayG;

	// Vortex

	displayG = FMath::RoundToInt(VerticalG);
	CalculateVortex();
}

void UFlightComponent::CalculateVortex() 
{
	if (displayG >= VORTEXG && previousGForce < VORTEXG) OnVortexActivate.Broadcast(true);
	else if (displayG < VORTEXG && previousGForce >= VORTEXG) OnVortexActivate.Broadcast(false);
}

// ====================================
// Throttle Stage for Acceleration and Target Speed
// ====================================

void UFlightComponent::SlowSpeed(float ThrottlePercentage)
{
	float negation = 1 - ThrottlePercentage;
	targetSpeed = 0;

	float scale = (ThrottlePercentage <= 0.1) ? 3.f : 1.25f;
	Acceleration = -(AircraftStats->Acceleration * negation * scale);
}

void UFlightComponent::NormalSpeed(float ThrottlePercentage)
{
	Acceleration = AircraftStats->Acceleration * ThrottlePercentage;
	targetSpeed = AircraftStats->MaxSpeed * 0.5f;
}

void UFlightComponent::AfterburnerSpeed(float ThrottlePercentage)
{
	if (ThrottlePercentage >= 0.9f)
	{
		Acceleration = AircraftStats->Acceleration * ThrottlePercentage * 3.f;
		targetSpeed = AircraftStats->MaxSpeed;
		return;
	}
	Acceleration = AircraftStats->Acceleration * ThrottlePercentage * 1.25f;
	targetSpeed = AircraftStats->MaxSpeed * 0.9f;
}

// ====================================
// Turning/Rotation Calculations
// ====================================

void UFlightComponent::ApplyRot(float DeltaSeconds)
{
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

void UFlightComponent::ReturnAOA(float DeltaSeconds)
{
	// Rotates the Root vector toward Airframe's vector
	if (!IsValid(Controlled)) return;
	FQuat CurrentQuat = Controlled->GetActorQuat();
	FQuat TargetQuat = Controlled->Airframe->GetComponentQuat();
	float RootTurnSpeed = 2.0f;
	float RootAlpha = FMath::Clamp(RootTurnSpeed * DeltaSeconds, 0.f, 1.f);
	FQuat NewRootQuat = FQuat::Slerp(CurrentQuat, TargetQuat, RootAlpha);
	if (IsValid(Controlled)) Controlled->SetActorRotation(NewRootQuat);

	// Resets the Airframe's vector 
	FQuat AirframeCurrentRelQuat = Controlled->Airframe->GetRelativeRotation().Quaternion();
	FQuat IdentityQuat = FQuat::Identity;
	float NoseAlpha = FMath::Clamp(AircraftStats->AOARecoverySpeed * DeltaSeconds, 0.f, 1.f);
	FQuat NewRelQuat = FQuat::Slerp(AirframeCurrentRelQuat, IdentityQuat, NoseAlpha);
	if (IsValid(Controlled) && IsValid(Controlled->Airframe)) Controlled->Airframe->SetRelativeRotation(NewRelQuat.Rotator());
}

float UFlightComponent::GetAOA() 
{
	if (!IsValid(Controlled)) return 0.f;
	FVector Forward = Controlled->Airframe->GetForwardVector().GetSafeNormal();
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
	float dragCo = Controlled->AirStats->DragCoefficient;
	float drag = FMath::Abs(GetAOA()) * dragCo * (GetCurrentSpeedKMH() / 4);

	//GEngine->AddOnScreenDebugMessage(-1, 0.01f, FColor::Yellow, FString::Printf(TEXT("Pitch: %.2f"), drag));

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
		Controlled->Airframe->AddWorldRotation(Rotate);
	}
}

float UFlightComponent::PitchDrag() 
{
	if (!IsValid(Controlled) || !IsValid(Controlled->Airframe)) return 0.f;
	FVector Up = Controlled->Airframe->GetUpVector();
	FVector Forward = Controlled->Airframe->GetForwardVector();

	float PitchAngle = FMath::RadiansToDegrees(
		FMath::Atan2(Forward.Z, FVector(Forward.X, Forward.Y, 0.f).Size())
	);

	float RadAngle = FMath::DegreesToRadians(PitchAngle);

	float UpDot = FVector::DotProduct(Up, FVector::UpVector);

	// TODO: Make this more robust
	return PitchAngle / 50;
}

// ====================================
// Apply Rotations
// ====================================

void UFlightComponent::ApplyPitch(float DeltaSeconds)
{
	if (!IsValid(Controlled)) return;
	if (bLanded && bCanTakeOff && UserPitch < 0) return;
	float CurveTurn = Controlled->AirStats->DragCurve->GetFloatValue(GetCurrentSpeedKMH());
	float CompressionTurn = Controlled->AirStats->CompressionCurve->GetFloatValue(GetCurrentSpeedKMH());
	//GEngine->AddOnScreenDebugMessage(-1, 0.01f, FColor::Yellow, FString::Printf(TEXT("Curve: %.2f"), CurveTurn));
	if (UserPitch == 0)
	{
		NextPitch = FMath::FInterpTo(NextPitch, 0, DeltaSeconds, 3.f);
		FRotator DeltaRot(NextPitch * DeltaSeconds, 0.f, 0.f);
		Controlled->Airframe->AddLocalRotation(DeltaRot);
		return;
	}
		
	float InterpSpeed = AircraftStats->TurnRate * CurveTurn * CompressionTurn;
	InterpSpeed = UserPitch < 0 ? InterpSpeed * 0.5 : InterpSpeed;

	float TargetPitchRate = UserPitch * InterpSpeed;
	NextPitch = FMath::FInterpTo(NextPitch, TargetPitchRate, DeltaSeconds, 5.f);

	FRotator DeltaRot(NextPitch * DeltaSeconds, 0.f, 0.f);
	if (IsValid(Controlled) && IsValid(Controlled->Airframe))
		Controlled->Airframe->AddLocalRotation(DeltaRot);
}

void UFlightComponent::ApplyYaw(float DeltaSeconds)
{
	if (!IsValid(Controlled)) return;
	if (UserYaw == 0) {
		NextYaw = FMath::FInterpTo(NextYaw, 0, DeltaSeconds, 3.f);
		FRotator DeltaRot(0.f, NextYaw * DeltaSeconds,0.f);
		Controlled->Airframe->AddLocalRotation(DeltaRot);
		return;
	}
	float TargetYawRate = UserYaw * AircraftStats->RudderRate;
	NextYaw = FMath::FInterpTo(NextYaw, TargetYawRate, DeltaSeconds, 2.f);

	FRotator DeltaRot(0.f, NextYaw * DeltaSeconds,0.f);
	if (IsValid(Controlled) && IsValid(Controlled->Airframe))
		Controlled->Airframe->AddLocalRotation(DeltaRot);
}

void UFlightComponent::ApplyRoll(float DeltaSeconds)
{
	if (!IsValid(Controlled)) return;
	if (UserRoll == 0) 
	{
		NextRoll = FMath::FInterpTo(NextRoll, 0, DeltaSeconds, 3.f);
		FRotator DeltaRot(0.f, 0.f, NextRoll * DeltaSeconds);
		Controlled->Airframe->AddLocalRotation(DeltaRot);
		return;
	}
	float TargetRollRate = UserRoll * AircraftStats->RollRate;
	NextRoll = FMath::FInterpTo(NextRoll, TargetRollRate, DeltaSeconds, 5.f);
	FRotator DeltaRot(0.f, 0.f, NextRoll * DeltaSeconds);
	if (IsValid(Controlled) && IsValid(Controlled->Airframe))
		Controlled->Airframe->AddLocalRotation(DeltaRot);
}