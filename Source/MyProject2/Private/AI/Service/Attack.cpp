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

void UBTServiceAttack::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) {
	if (!Controlled || !BlackboardComp || !Selected) return;
	CalculateAngle(DeltaSeconds);
	PitchAngle();
}

void UBTServiceAttack::CalculateAngle(float DeltaSeconds)
{
	float neededRoll = CalculateRollDegrees();
	neededRoll = FMath::Clamp(neededRoll, -1, 1);
	if (FMath::Abs(neededRoll) < 0.1) {
		BlackboardComp->SetValueAsFloat(RollKey.SelectedKeyName, 0);
	}
	else {
		BlackboardComp->SetValueAsFloat(RollKey.SelectedKeyName, neededRoll);
	}

	if (GEngine)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Yellow,
		//	FString::Printf(TEXT("Roll Input: %f"),neededRoll));
	}
}

// 180 to -180 degrees
float UBTServiceAttack::CalculateRollDegrees()
{
	FVector Forward = Controlled->Airframe->GetForwardVector();
	FVector Up = Controlled->Airframe->GetUpVector();

	FVector ToTarget = (Selected->GetActorLocation() - Controlled->GetActorLocation()).GetSafeNormal();
	if (ToTarget.IsNearlyZero()) return 0;

	FVector ToTargetPlane = ToTarget - FVector::DotProduct(ToTarget, Forward) * Forward;
	if (ToTargetPlane.IsNearlyZero()) return 0;
	ToTargetPlane.Normalize();

	float RollRad = FMath::Atan2(
		FVector::DotProduct(FVector::CrossProduct(ToTargetPlane, Up), Forward),
		FVector::DotProduct(Up, ToTargetPlane)
	);

	return FMath::RadiansToDegrees(RollRad);
}

void UBTServiceAttack::PitchAngle() 
{
	FVector TrackingLocation = Selected->GetActorLocation();
	FVector DistanceBetween = (TrackingLocation - Controlled->GetActorLocation());
	if (DistanceBetween.IsNearlyZero()) return;
	DistanceBetween.Normalize();
	FVector Forward = Controlled->GetActorForwardVector();
	float PitchAngle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(Forward, DistanceBetween)));
	float Sign = FMath::Sign(FVector::DotProduct(DistanceBetween, Controlled->GetActorUpVector()));
	float PitchOffset = PitchAngle * Sign;

	BlackboardComp->SetValueAsFloat(PitchKey.SelectedKeyName, PitchOffset);

	if (GEngine)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("Pitch: %f"), PitchOffset));
	}
}