// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Attack!"));
#include "AI/Service/Attack.h"
#include "Aircraft/AI/EnemyAircraft.h"
#include "Aircraft/FlightComponent.h"
#include "Structs and Data/Aircraft Data/AircraftStats.h"
#include "Aircraft/AI/EnemyAircraftAI.h"

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
	if (Controller)
	{
		Controlled = Cast<AEnemyAircraft>(Controller->Controlled);
	}
	Selected = Cast<AActor>(BlackboardComp->GetValueAsObject(TargetActorKey.SelectedKeyName));
}

void UBTServiceAttack::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) 
{
	if (!Controlled || !BlackboardComp || !Selected) return;
	CalculateAngle(DeltaSeconds);
}

void UBTServiceAttack::CalculateAngle(float DeltaSeconds)
{
	float PitchErrorDeg = CalculatePitchDegrees();
	if (FMath::Abs(PitchErrorDeg) < 2) {
		PitchErrorDeg = 0.f;
	}
	float YawErrorDeg = CalculateYawDegrees();

	float pitchInput = PitchErrorDeg / 45.f;
	pitchInput = FMath::Clamp(pitchInput, -1, 1);
	if (FMath::Abs(pitchInput) < 0.05f) {
		pitchInput = 0.f;
	}

	BlackboardComp->SetValueAsFloat(PitchKey.SelectedKeyName, pitchInput);
	float neededRoll = CalculateRollDegrees(PitchErrorDeg, YawErrorDeg);

	neededRoll = FMath::Clamp(neededRoll, -1, 1);
	if (FMath::Abs(neededRoll) < 0.1) 
	{
		BlackboardComp->SetValueAsFloat(RollKey.SelectedKeyName, 0);
	}
	else
	{
		BlackboardComp->SetValueAsFloat(RollKey.SelectedKeyName, neededRoll);
	}

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Yellow,
			FString::Printf(TEXT("Roll Input: %f"),neededRoll));
	}
}

// 180 to -180 degrees
float UBTServiceAttack::CalculateRollDegrees(float CurrentPitchErrorDeg, float CurrentYawErrorDeg)
{
	if (FMath::Abs(CurrentPitchErrorDeg) > 60.f)
		return 0.f;

	float rollInput = CurrentYawErrorDeg / 45.f; 
	rollInput = FMath::Clamp(rollInput, -1.f, 1.f);

	if (FMath::Abs(rollInput) < 0.05f)
		rollInput = 0.f;

	return rollInput;
}

float UBTServiceAttack::CalculatePitchDegrees() 
{
	FVector ToTarget = (Selected->GetActorLocation() - Controlled->GetActorLocation());
	if (ToTarget.IsNearlyZero()) return 0.f;
	ToTarget.Normalize();

	FVector Forward = Controlled->Airframe->GetForwardVector();
	FVector Up = Controlled->Airframe->GetUpVector();

	FVector ToTargetPlane = ToTarget - FVector::DotProduct(ToTarget, Controlled->Airframe->GetRightVector()) * Controlled->Airframe->GetRightVector();
	if (ToTargetPlane.IsNearlyZero()) return 0.f;
	ToTargetPlane.Normalize();

	float PitchRad = FMath::Atan2(
		FVector::DotProduct(ToTargetPlane, Up),
		FVector::DotProduct(ToTargetPlane, Forward)
	);

	if (GEngine)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Green,
		//	FString::Printf(TEXT("Pitch: %f"), PitchRad));
	}

	return FMath::RadiansToDegrees(PitchRad);
}

float UBTServiceAttack::CalculateYawDegrees()
{
	FVector ToTarget = (Selected->GetActorLocation() - Controlled->GetActorLocation());
	if (ToTarget.IsNearlyZero()) return 0.f;
	ToTarget.Normalize();

	FVector Forward = Controlled->Airframe->GetForwardVector();
	FVector Right = Controlled->Airframe->GetRightVector();

	FVector FlatForward = FVector(Forward.X, Forward.Y, 0.f).GetSafeNormal();
	FVector FlatToTarget = FVector(ToTarget.X, ToTarget.Y, 0.f).GetSafeNormal();

	if (FlatForward.IsNearlyZero() || FlatToTarget.IsNearlyZero())
		return 0.f;

	float YawRad = FMath::Atan2(
		FVector::DotProduct(FlatToTarget, Right),
		FVector::DotProduct(FlatToTarget, FlatForward)
	);

	return FMath::RadiansToDegrees(YawRad);
}