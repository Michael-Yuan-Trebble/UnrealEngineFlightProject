// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Flight Component!"));
#include "Units/Aircraft/FlightComponent.h"
#include "Units/Aircraft/Player/PlayerAircraft.h"


EThrottleStage UFlightComponent::getThrottleStage(const float throttle)
{
	if (throttle >= 0 && throttle <= 0.4) return EThrottleStage::Slow;
	else if (throttle <= 0.8) return EThrottleStage::Normal;
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
		if (IsValid(AircraftStats)) FlightDrag = AircraftStats->Acceleration * 0.5;
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
	if (IsAirframeValid() || !AircraftStats) return;

	// ====================================
	// Movement Application Components
	// ====================================

	if (FlightMode != EFlightMode::Flight) CheckLanding(DeltaTime);

	ApplySpeed(CurrentThrust, DeltaTime);

	if (FlightMode == EFlightMode::Naval && bLanded) return;

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
			// TODO: Stall should enter a state where it goes down, but also skips that drop speed process because it feels clunky
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

	if (FlightMode == EFlightMode::Naval && bLanded) return;

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

	if (IsControlledValid()) return;

	if (bLanded && !bCanTakeOff) { 
		Velocity.Z = 0; 
	}

	Controlled->AddActorWorldOffset(Velocity * DeltaSeconds, true);
	CalculateGForce(DeltaSeconds);
}

void UFlightComponent::AddSpeed(const float Speed, const float D) {
	currentStage = getThrottleStage(CurrentThrust);
	switch (currentStage)
	{
		case EThrottleStage::Slow: SlowSpeed(CurrentThrust); break;
		case EThrottleStage::Normal: NormalSpeed(CurrentThrust); break;
		case EThrottleStage::Afterburner: AfterburnerSpeed(CurrentThrust); break;
	}

	currentSpeed += ConvertKMHToSpeed(Speed) + Acceleration;
	if (!FMath::IsFinite(currentSpeed) || currentSpeed < 0.f) currentSpeed = 0;
	currentSpeed = FMath::Clamp(currentSpeed, 0, 1000000);

	PreviousVelocity = Velocity;
	if (bLanded && IsValid(Controlled->Airframe)) Velocity = Controlled->Airframe->GetForwardVector() * currentSpeed;
	else Velocity = Controlled->GetActorForwardVector() * currentSpeed;
	if (Velocity.ContainsNaN() || Velocity.Size() > 1e6f) Velocity = FVector::ZeroVector;

	if (IsControlledValid()) return;

	if (bLanded && !bCanTakeOff) {
		Velocity.Z = 0;
	}

	Controlled->AddActorWorldOffset(Velocity * D, true);
	CalculateGForce(D);
}

void UFlightComponent::CheckLanding(const float D)
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

void UFlightComponent::AddDropSpeed(const float D) {

	// It doesn't have to go to 90! it can go to something like 30 or 40, once stalling then push it to 90 for the user to gain speed then exit stall

	if (IsAirframeValid()) return;
	if (GetCurrentSpeedKMH() <= StallSpeed) return;

	float MaxDropAngle = 30.f;

	float Speed = GetCurrentSpeedKMH();
	float Percent = FMath::Clamp(Speed / DropSpeed, 0.f, 1.f);
	
	float CurrentMaxAngleDrop = MaxDropAngle * (1 - Percent);

	TempRecovery(D, CurrentMaxAngleDrop);
}

void UFlightComponent::Stall(const float D)
{
	if (!IsValid(Controlled) || !IsValid(Controlled->Airframe)) return;
	FQuat AirframeCurrentRelQuat = Controlled->Airframe->GetRelativeRotation().Quaternion();
	FQuat IdentityQuat = FQuat::Identity;
	float NoseAlpha = FMath::Clamp(AircraftStats->AOARecoverySpeed * D, 0.f, 1.f);
	FQuat NewRelQuat = FQuat::Slerp(AirframeCurrentRelQuat, IdentityQuat, NoseAlpha);
	if (IsValid(Controlled) && IsValid(Controlled->Airframe)) Controlled->Airframe->SetRelativeRotation(NewRelQuat.Rotator());
}

void UFlightComponent::Landed(const float D)
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

void UFlightComponent::CalculateGForce(const float DeltaSeconds)
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
	if (displayG >= VortexG && previousGForce < VortexG) OnVortexActivate.Broadcast(true);
	else if (displayG < VortexG && previousGForce >= VortexG) OnVortexActivate.Broadcast(false);
}

// ====================================
// Throttle Stage for Acceleration and Target Speed
// ====================================

void UFlightComponent::SlowSpeed(const float ThrottlePercentage)
{
	float negation = 1 - ThrottlePercentage;
	targetSpeed = 0;

	float scale = (ThrottlePercentage <= 0.1) ? 3.f : 1.25f;
	Acceleration = -(AircraftStats->Acceleration * negation * scale);
}

void UFlightComponent::NormalSpeed(const float ThrottlePercentage)
{
	Acceleration = AircraftStats->Acceleration * ThrottlePercentage;
	targetSpeed = AircraftStats->MaxSpeed * 0.5f;
}

void UFlightComponent::AfterburnerSpeed(const float ThrottlePercentage)
{
	if (ThrottlePercentage >= 0.9f)
	{
		Acceleration = AircraftStats->Acceleration * ThrottlePercentage * 2.f;
		targetSpeed = AircraftStats->MaxSpeed;
		return;
	}
	Acceleration = AircraftStats->Acceleration * ThrottlePercentage * 1.25f;
	targetSpeed = AircraftStats->MaxSpeed * 0.9f;
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
	if (IsControlledValid()) return;
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

void UFlightComponent::TempRecovery(const float D, const float Deg)
{
	if (IsAirframeValid()) return;

	FQuat CurrentRootQuat = Controlled->GetActorQuat();
	FQuat AirframeQuat = Controlled->Airframe->GetComponentQuat();

	FVector WorldForward = FVector::ForwardVector;
	FVector PitchAxis = FVector::CrossProduct(WorldForward, FVector::UpVector).GetSafeNormal();
	FQuat DownOffset = FQuat(PitchAxis, FMath::DegreesToRadians(-Deg));

	FQuat TargetRootQuat = AirframeQuat * DownOffset;

	float RootAlpha = FMath::Clamp(2.0f * D, 0.f, 1.f);
	FQuat NewRootQuat = FQuat::Slerp(CurrentRootQuat, TargetRootQuat, RootAlpha);
	Controlled->SetActorRotation(NewRootQuat);

	FQuat AirframeCurrentRelQuat = Controlled->Airframe->GetRelativeRotation().Quaternion();

	FQuat DesiredRelQuat = DownOffset.Inverse();

	float NoseAlpha = FMath::Clamp(AircraftStats->AOARecoverySpeed * D, 0.f, 1.f);
	FQuat NewAirframeRelQuat = FQuat::Slerp(AirframeCurrentRelQuat, DesiredRelQuat, NoseAlpha);

	Controlled->Airframe->SetRelativeRotation(NewAirframeRelQuat);
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

void UFlightComponent::RollAOA(const float DeltaSeconds)
{
	// ====================================
	// Apply a downward AOA based on user's roll orientation with the ground
	// ====================================

	// TODO: Make this do it on root not airframe

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

void UFlightComponent::ApplyPitch(const float DeltaSeconds)
{
	if (!IsValid(Controlled)) return;
	if (bLanded && bCanTakeOff && UserRotation.Pitch < 0) return;
	float CurveTurn = Controlled->AirStats->DragCurve->GetFloatValue(GetCurrentSpeedKMH());
	float CompressionTurn = Controlled->AirStats->CompressionCurve->GetFloatValue(GetCurrentSpeedKMH());
	//GEngine->AddOnScreenDebugMessage(-1, 0.01f, FColor::Yellow, FString::Printf(TEXT("Curve: %.2f"), CurveTurn));
	if (UserRotation.Pitch == 0)
	{
		NextRotation.Pitch = FMath::FInterpTo(NextRotation.Pitch, 0, DeltaSeconds, 3.f);
		FRotator DeltaRot(NextRotation.Pitch * DeltaSeconds, 0.f, 0.f);
		Controlled->Airframe->AddLocalRotation(DeltaRot);
		return;
	}
		
	float InterpSpeed = AircraftStats->TurnRate * CurveTurn * CompressionTurn;
	InterpSpeed = UserRotation.Pitch < 0 ? InterpSpeed * 0.5 : InterpSpeed;

	float TargetPitchRate = UserRotation.Pitch * InterpSpeed;
	NextRotation.Pitch = FMath::FInterpTo(NextRotation.Pitch, TargetPitchRate, DeltaSeconds, 5.f);

	FRotator DeltaRot(NextRotation.Pitch * DeltaSeconds, 0.f, 0.f);
	if (IsValid(Controlled) && IsValid(Controlled->Airframe))
		Controlled->Airframe->AddLocalRotation(DeltaRot);
}

void UFlightComponent::ApplyYaw(const float DeltaSeconds)
{
	if (IsControlledValid()) return;
	if (UserRotation.Yaw == 0) {
		NextRotation.Yaw = FMath::FInterpTo(NextRotation.Yaw, 0, DeltaSeconds, 3.f);
		FRotator DeltaRot(0.f, NextRotation.Yaw * DeltaSeconds,0.f);
		Controlled->Airframe->AddLocalRotation(DeltaRot);
		return;
	}
	float TargetYawRate = UserRotation.Yaw * AircraftStats->RudderRate;
	NextRotation.Yaw = FMath::FInterpTo(NextRotation.Yaw, TargetYawRate, DeltaSeconds, 2.f);

	FRotator DeltaRot(0.f, NextRotation.Yaw * DeltaSeconds,0.f);
	if (IsValid(Controlled) && IsValid(Controlled->Airframe)) Controlled->Airframe->AddLocalRotation(DeltaRot);
}

void UFlightComponent::ApplyRoll(const float DeltaSeconds)
{
	if (IsControlledValid()) return;
	if (UserRotation.Roll == 0)
	{
		NextRotation.Roll = FMath::FInterpTo(NextRotation.Roll, 0, DeltaSeconds, 3.f);
		FRotator DeltaRot(0.f, 0.f, NextRotation.Roll * DeltaSeconds);
		Controlled->Airframe->AddLocalRotation(DeltaRot);
		return;
	}
	float TargetRollRate = UserRotation.Roll * AircraftStats->RollRate;
	NextRotation.Roll = FMath::FInterpTo(NextRotation.Roll, TargetRollRate, DeltaSeconds, 5.f);
	FRotator DeltaRot(0.f, 0.f, NextRotation.Roll * DeltaSeconds);
	if (IsValid(Controlled) && IsValid(Controlled->Airframe)) Controlled->Airframe->AddLocalRotation(DeltaRot);
}