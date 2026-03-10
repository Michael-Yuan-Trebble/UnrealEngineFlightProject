// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/Service/Attack.h"
#include "Structs and Data/MathLib/FlightMathLibrary.h"
#include "Units/Aircraft/BaseAircraft.h"
#include "AI/AircraftAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Debug/DebugHelper.h"

EAIThrottleMode UBTServiceAttack::GetThrottleMode(float distance) 
{
	// Makes the distance into meters
	distance = distance * 0.01;
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
	Controller = Cast<AAircraftAIController>(OwnerComp.GetAIOwner());
	if (IsValid(Controller)) Controlled = Cast<ABaseAircraft>(Controller->GetControlled());
	if (IsValid(BlackboardComp)) Selected = Cast<AActor>(BlackboardComp->GetValueAsObject(TargetActorKey.SelectedKeyName));
}

void UBTServiceAttack::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) 
{
	CalculateAngle(DeltaSeconds);
	CalculateThrust(DeltaSeconds);
}

void UBTServiceAttack::CalculateAngle(const float DeltaSeconds)
{
	AActor* Target = Selected.Get();
	if (!IsValid(Target) || !IsValid(Controlled)) return;
	FVector ToTargetWorld = (Target->GetActorLocation() - Controlled->GetActorLocation()).GetSafeNormal();
	if (ToTargetWorld.IsNearlyZero()) return;

	FTransform AirframeTransform = Controlled->GetAirframe()->GetComponentTransform();
	FVector LocalDir = AirframeTransform.InverseTransformVectorNoScale(ToTargetWorld);

	float DesiredRollInput = CalculateRollDegrees(LocalDir);

	// TODO: Change this later, for now this is temp here for preventing rolling and pitching funkiness
	float DesiredPitchInput = 0.f;
	if (DesiredRollInput < 0.1) 
	{
		DesiredPitchInput = CalculatePitchDegrees(LocalDir);
	}
	float DesiredYawInput = CalculateYawDegrees(LocalDir);

	if (!IsValid(BlackboardComp)) return;

	BlackboardComp->SetValueAsFloat(RollKey.SelectedKeyName, -DesiredRollInput);
	BlackboardComp->SetValueAsFloat(PitchKey.SelectedKeyName, DesiredPitchInput);
	BlackboardComp->SetValueAsFloat(YawKey.SelectedKeyName, DesiredYawInput);
}

float UBTServiceAttack::CalculateRollDegrees(const FVector& LocalDir)
{
	float YawErrorRad = FMath::Atan2(LocalDir.Y, LocalDir.X);

	float YawErrorDeg = FMath::RadiansToDegrees(YawErrorRad);

	float DesiredRollInput = FMath::Clamp(-YawErrorDeg * RollGain, -1.f, 1.f);

	if (FMath::Abs(YawErrorDeg) < 0.5f) DesiredRollInput = 0.f;
	return DesiredRollInput;
}

float UBTServiceAttack::CalculatePitchDegrees(const FVector& LocalDir)
{
	float PitchErrorRad = FMath::Atan2(LocalDir.Z, LocalDir.X);

	float PitchErrorDeg = FMath::RadiansToDegrees(PitchErrorRad);

	float DesiredPitchInput = FMath::Clamp(PitchErrorDeg * PitchGain, -1.f, 1.f);

	const float ErrorDeadzoneDeg = 1.0f;
	if (FMath::Abs(PitchErrorDeg) < ErrorDeadzoneDeg) DesiredPitchInput = 0.f;

	return DesiredPitchInput;
}

float UBTServiceAttack::CalculateYawDegrees(const FVector& LocalDir)
{
	float YawErrorRad = FMath::Atan2(LocalDir.Y, LocalDir.X);

	float YawErrorDeg = FMath::RadiansToDegrees(YawErrorRad);

	float DesiredYawInput = FMath::Clamp(YawErrorDeg * YawGain, -1.f, 1.f);

	const float ErrorDeadzoneDeg = 1.0f;
	if (FMath::Abs(YawErrorDeg) < ErrorDeadzoneDeg) DesiredYawInput = 0.f;

	return DesiredYawInput;
}

void UBTServiceAttack::CalculateThrust(const float DeltaSeconds) 
{
	AActor* Target = Selected.Get();

	if (!IsValid(Target) || !IsValid(Controlled)) return;

	float Distance = FVector::Dist(Controlled->GetActorLocation(), Selected.Get()->GetActorLocation());

	float throttle = 0.f;
	EAIThrottleMode throttleMode = GetThrottleMode(Distance);
	if (Target) 
	{
		// Make Speed dependent on how many aircraft are in a radius, keep speed high when there are more enemies
		// For now its single target
		switch (throttleMode) 
		{
			case EAIThrottleMode::FarAway: 
				throttle = 1.f; 
				break;
			case EAIThrottleMode::MidRange: 
				throttle = 0.5f;
				break;
			case EAIThrottleMode::Close: 
				throttle = PursuitThrottle(Cast<ABaseAircraft>(Target));
				break;
			default:
				throttle = 0.f;
				break;
		}
	}
	else
	{
		// For anything that isn't an aircraft, maintain some relative speed, it shouldn't really change its speed that much
		switch (throttleMode) 
		{
			case EAIThrottleMode::FarAway: 
				throttle = 0.8f;
				break;
			case EAIThrottleMode::MidRange: 
				throttle = 0.5f;
				break;
			case EAIThrottleMode::Close: 
				throttle = 0.5f;
				break;
			default:
				throttle = 0.f;
				break;
		}
	}
	if (BlackboardComp) BlackboardComp->SetValueAsFloat(ThrottleKey.SelectedKeyName, throttle);
}

float UBTServiceAttack::PursuitThrottle(ABaseAircraft* Target)
{
	if (!IsValid(Target) || !IsValid(Controlled)) return 0.5f;
	float targetSpeed = Target->GetKMHSpeed();
	float currentSpeed = Controlled->GetKMHSpeed();
	float speedDif = targetSpeed - currentSpeed;
	float absSpeedDif = FMath::Abs(speedDif);

	// Maybe make these ranges dependent based on pilot skill
	if (absSpeedDif <= 100) 
	{
		return 0.5f;
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