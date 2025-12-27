// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Attack!"));
#include "AI/Service/Attack.h"
#include "Units/Aircraft/AI/EnemyAircraft.h"
#include "Units/Aircraft/FlightComponent.h"
#include "Structs and Data/Aircraft Data/AircraftStats.h"
#include "Units/Aircraft/AI/EnemyAircraftAI.h"
#include "DrawDebugHelpers.h"

EAIThrottleMode UBTServiceAttack::GetThrottleMode(float distance) 
{
	// Makes the distance into meters
	distance = distance * 0.034;
	if (distance >= 7000) return EAIThrottleMode::FarAway;
	else if (distance >= 3000 && distance < 7000) return EAIThrottleMode::MidRange;
	else return EAIThrottleMode::Close;
}

UBTServiceAttack::UBTServiceAttack()
{
	NodeName = "Update Attack Movement";
	Interval = 0.2f;
	bNotifyBecomeRelevant = true;
}

void UBTServiceAttack::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) 
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);
	BlackboardComp = OwnerComp.GetBlackboardComponent();
	Controller = Cast<AEnemyAircraftAI>(OwnerComp.GetAIOwner());
	if (Controller) Controlled = Cast<AEnemyAircraft>(Controller->Controlled);
	Selected = Cast<AActor>(BlackboardComp->GetValueAsObject(TargetActorKey.SelectedKeyName));
}

void UBTServiceAttack::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) 
{
	if (!Controlled || !Controlled->FlightComponent ||!BlackboardComp || !Selected) return;
	CalculateAngle(DeltaSeconds);
	CalculateThrust(DeltaSeconds);
}

void UBTServiceAttack::CalculateAngle(float DeltaSeconds)
{
	FVector ToTargetWorld = (Selected->GetActorLocation() - Controlled->GetActorLocation()).GetSafeNormal();
	if (ToTargetWorld.IsNearlyZero()) return;

	FTransform AirframeTransform = Controlled->Airframe->GetComponentTransform();
	FVector LocalDir = AirframeTransform.InverseTransformVectorNoScale(ToTargetWorld);

	float DesiredRollInput = CalculateRollDegrees(LocalDir);

	// TODO: Change this later, for now this is temp here for preventing rolling and pitching funkiness
	float DesiredPitchInput = 0.f;
	if (DesiredRollInput < 0.1) 
	{
		DesiredPitchInput = CalculatePitchDegrees(LocalDir);
	}
	float DesiredYawInput = CalculateYawDegrees(LocalDir);

	GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Cyan,FString::Printf(TEXT("Pitch: %.2f Roll: %.2f"), DesiredPitchInput, DesiredRollInput));

	BlackboardComp->SetValueAsFloat(RollKey.SelectedKeyName, -DesiredRollInput);
	BlackboardComp->SetValueAsFloat(PitchKey.SelectedKeyName, DesiredPitchInput);
	BlackboardComp->SetValueAsFloat(YawKey.SelectedKeyName, DesiredYawInput);
}

float UBTServiceAttack::CalculateRollDegrees(FVector LocalDir)
{
	float YawErrorRad = FMath::Atan2(LocalDir.Y, LocalDir.X);

	float YawErrorDeg = FMath::RadiansToDegrees(YawErrorRad);

	float DesiredRollInput = FMath::Clamp(-YawErrorDeg * RollGain, -1.f, 1.f);

	if (FMath::Abs(YawErrorDeg) < 0.5f) DesiredRollInput = 0.f;
	return DesiredRollInput;
}

float UBTServiceAttack::CalculatePitchDegrees(FVector LocalDir)
{
	float PitchErrorRad = FMath::Atan2(LocalDir.Z, LocalDir.X);

	float PitchErrorDeg = FMath::RadiansToDegrees(PitchErrorRad);

	float DesiredPitchInput = FMath::Clamp(PitchErrorDeg * PitchGain, -1.f, 1.f);

	const float ErrorDeadzoneDeg = 1.0f;
	if (FMath::Abs(PitchErrorDeg) < ErrorDeadzoneDeg) DesiredPitchInput = 0.f;

	return DesiredPitchInput;
}

float UBTServiceAttack::CalculateYawDegrees(FVector LocalDir)
{
	float YawErrorRad = FMath::Atan2(LocalDir.Y, LocalDir.X);

	float YawErrorDeg = FMath::RadiansToDegrees(YawErrorRad);

	float DesiredYawInput = FMath::Clamp(YawErrorDeg * YawGain, -1.f, 1.f);

	const float ErrorDeadzoneDeg = 1.0f;
	if (FMath::Abs(YawErrorDeg) < ErrorDeadzoneDeg) DesiredYawInput = 0.f;

	return DesiredYawInput;
}

void UBTServiceAttack::CalculateThrust(float DeltaSeconds) 
{
	ABaseAircraft* Target = Cast<ABaseAircraft>(Selected);
	if (Target) 
	{
		// Make Speed dependent on how many aircraft are in a radius, keep speed high when there are more enemies
		// For now its single target
		float Distance = FVector::Dist(Controlled->GetActorLocation(), Selected->GetActorLocation());
		//GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Cyan, FString::Printf(TEXT("Distance: %.2f"), Distance));

		// Maybe make this a throttle range in the future instead of just these hard values
		float throttle = 0.f;
		EAIThrottleMode throttleMode = GetThrottleMode(Distance);
		switch (throttleMode) 
		{
			case EAIThrottleMode::FarAway: throttle = 1;
			case EAIThrottleMode::MidRange: throttle = 0.5;
			case EAIThrottleMode::Close: throttle = PursuitThrottle(Target);
		}

		BlackboardComp->SetValueAsFloat(ThrottleKey.SelectedKeyName, throttle);
	}
	else
	{
		// For anything that isn't an aircraft, maintain some relative speed, it shouldn't really change its speed that much
		float Distance = FVector::Dist(Controlled->GetActorLocation(), Selected->GetActorLocation());
		float throttle = 0.f;
		EAIThrottleMode throttleMode = GetThrottleMode(Distance);
		switch (throttleMode) 
		{
			case EAIThrottleMode::FarAway: throttle = 0.8;
			case EAIThrottleMode::MidRange: throttle = 0.5;
			case EAIThrottleMode::Close: throttle = 0.5;
		}
		BlackboardComp->SetValueAsFloat(ThrottleKey.SelectedKeyName, throttle);
	}
}

float UBTServiceAttack::PursuitThrottle(ABaseAircraft* Target)
{
	float targetSpeed = Target->FlightComponent->currentSpeed;
	float currentSpeed = Controlled->FlightComponent->currentSpeed;
	float speedDif = targetSpeed - currentSpeed;
	float absSpeedDif = FMath::Abs(speedDif);

	// Maybe make these ranges dependent based on pilot skill
	if (absSpeedDif <= 100) 
	{
		return 0.5;
	}
	// These are temporary setting speeds, plan is to make it more dynamic use more ranges of throttle percentages.
	if (speedDif < 0)
	{
		if (absSpeedDif >= 400) 
		{
			return 0.1f;
		}
		return 0.3f;
	}
	if (speedDif > 0)
	{
		if (absSpeedDif >= 400) 
		{
			return 0.8f;
		}
		return 0.6f;
	}
	return 0.f;
}