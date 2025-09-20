// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Attack!"));
#include "AI/Service/Attack.h"
#include "Aircraft/AI/EnemyAircraft.h"
#include "Aircraft/AI/EnemyAircraftAI.h"

UBTServiceAttack::UBTServiceAttack() {
	NodeName = "Update Attack Movement";
	Interval = 0.2f;
	bNotifyBecomeRelevant = true;
}

void UBTServiceAttack::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) {
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
	FVector TrackingLocation = Selected->GetActorLocation();

	FVector Forward = Controlled->GetActorForwardVector();
	FVector CurrentUp = Controlled->GetActorUpVector();
	FVector ToTarget = (TrackingLocation - Controlled->GetActorLocation());

	if (ToTarget.IsNearlyZero()) return;
	ToTarget.Normalize();

	// Project target vector onto plane perpendicular to Forward (lateral error)
	FVector Lateral = ToTarget - (FVector::DotProduct(ToTarget, Forward) * Forward);
	if (Lateral.IsNearlyZero()) return;
	Lateral.Normalize();

	// Roll correction: how much to tilt wings so "up" points toward target plane
	float RollAmount = FVector::DotProduct(FVector::CrossProduct(CurrentUp, Lateral), Forward);

	// Clamp
	RollAmount = FMath::Clamp(RollAmount, -1.f, 1.f);

	// Deadzone
	if (FMath::Abs(RollAmount) < 0.05f)
	{
		RollAmount = 0.f;
	}

	// Smooth it out
	float CurrentRoll = BlackboardComp->GetValueAsFloat(RollKey.SelectedKeyName);
	float SmoothedRoll = FMath::FInterpTo(CurrentRoll, RollAmount, DeltaSeconds, 2.f);

	BlackboardComp->SetValueAsFloat(RollKey.SelectedKeyName, SmoothedRoll);

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("Roll: %f"), RollAmount));
	}
	
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