// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Flight Component!"));
#include "FlightComponent.h"

//power = (log10(20 / (0.07 * 1.225))) / (log10(ListedMaximumSpeed));

enum class ThrottleStage
{
	Slow,
	Normal,
	Afterburner
};

ThrottleStage getThrottleStage(float throttle)
{
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

	DrawDebugLine(GetWorld(), Controlled->GetActorLocation(), Controlled->GetActorLocation() + Forward * 300, FColor::Blue, false, 0.f, 0, 2.f);
	DrawDebugLine(GetWorld(), Controlled->GetActorLocation(), Controlled->GetActorLocation() + Controlled->Airframe->GetForwardVector() * 300, FColor::Green, false, 0.f, 0, 2.f);
}

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

void UFlightComponent::ApplyRot(float DeltaSeconds)
{
	if (!Controlled || !AircraftStats) return;
	ApplyYaw(DeltaSeconds);
	ApplyRoll(DeltaSeconds);
	ApplyPitch(DeltaSeconds);

	FQuat DeltaRotation = isFlying ? FQuat(FRotator(NextPitch, NextYaw, NextRoll))
		: FQuat(FRotator(0, NextYaw, 0));

	Controlled->Airframe->AddLocalRotation(DeltaRotation);

	ReturnAOA(DeltaSeconds);
}

void UFlightComponent::ReturnAOA(float DeltaSeconds)
{
	FQuat CurrentQuat = Controlled->GetActorQuat();
	FQuat TargetQuat = Controlled->Airframe->GetComponentQuat();
	float RootTurnSpeed = 2.0f;
	float RootAlpha = FMath::Clamp(RootTurnSpeed * DeltaSeconds, 0.f, 1.f);
	FQuat NewRootQuat = FQuat::Slerp(CurrentQuat, TargetQuat, RootAlpha);
	Controlled->SetActorRotation(NewRootQuat);

	FQuat AirframeCurrentRelQuat = Controlled->Airframe->GetRelativeRotation().Quaternion();
	FQuat IdentityQuat = FQuat::Identity;
	float NoseAlpha = FMath::Clamp(AircraftStats->AOARecoverySpeed * DeltaSeconds, 0.f, 1.f);
	FQuat NewRelQuat = FQuat::Slerp(AirframeCurrentRelQuat, IdentityQuat, NoseAlpha);
	Controlled->Airframe->SetRelativeRotation(NewRelQuat.Rotator());
}

void UFlightComponent::AdjustSpringArm(float DeltaSeconds, float ThrottlePercentage)
{
	if (!AircraftStats) return;
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

void UFlightComponent::RollAOA(float DeltaSeconds) {
	FVector Forward = Controlled->Airframe->GetForwardVector();
	FVector Up = Controlled->Airframe->GetUpVector();

	float PitchAngle = FMath::RadiansToDegrees(
		FMath::Atan2(Forward.Z, FVector(Forward.X, Forward.Y, 0.f).Size())
	);

	float UpDot = FVector::DotProduct(Up, FVector::UpVector);

	float DownPitchStrength = FMath::Clamp(1.f - UpDot, 0.f, 2.f) * 2.f;

	 DownPitch = -DownPitchStrength * DeltaSeconds;

	FVector WorldPitchAxis = FVector::CrossProduct(Forward, FVector::UpVector).GetSafeNormal();

	if (PitchAngle > -85.f)
	{
		FQuat Rotate = FQuat(WorldPitchAxis, FMath::DegreesToRadians(DownPitch));
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Pitch: %.2f"), DownPitch));
		Controlled->Airframe->AddWorldRotation(Rotate);
	}
}

void UFlightComponent::ApplyPitch(float DeltaSeconds)
{
	if (FMath::IsNearlyZero(UserPitch, 0.01f))
	{
		NextPitch = FMath::FInterpTo(NextPitch, 0, DeltaSeconds, 5.f);
	}
	else
	{
		NextPitch = FMath::FInterpTo(NextPitch, UserPitch, DeltaSeconds, 8.f);
		NextPitch = FMath::Clamp(NextPitch, -AircraftStats->TurnRate, AircraftStats->TurnRate);

		if (UserPitch < 0.01) NextPitch *= 0.7f;
	}
}

void UFlightComponent::ApplyYaw(float DeltaSeconds)
{
	if (UserYaw == 0) NextYaw = FMath::FInterpTo(NextYaw, 0, DeltaSeconds, 7.f);
	else
	{
		NextYaw = FMath::FInterpTo(NextYaw, UserYaw, DeltaSeconds, 2.f);
		NextYaw = FMath::Clamp(NextYaw, -AircraftStats->RudderRate, AircraftStats->RudderRate);
	}
}

void UFlightComponent::ApplyRoll(float DeltaSeconds)
{
	if (UserRoll == 0) NextRoll = FMath::FInterpTo(NextRoll, 0, DeltaSeconds, 3.5f);
	else
	{
		NextRoll = FMath::FInterpTo(NextRoll, UserRoll, DeltaSeconds, 8.f);
		NextRoll = FMath::Clamp(NextRoll, -AircraftStats->RollRate, AircraftStats->RollRate);
	}
}

void UFlightComponent::SetPitch(float PitchValue)
{
	UserPitch = PitchValue;
}

void UFlightComponent::SetYaw(float YawValue)
{
	UserYaw = YawValue;
}

void UFlightComponent::SetRoll(float RollValue)
{
	UserRoll = RollValue;
}

void UFlightComponent::SetThrust(float Thrust) {
	CurrentThrust = Thrust;
}