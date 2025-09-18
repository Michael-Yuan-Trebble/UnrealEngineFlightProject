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
	CalculateAngle();
	PitchAngle();
}

void UBTServiceAttack::CalculateAngle() 
{
	FVector TrackingLocation = Selected->GetActorLocation();

	FVector CurrentUp = Controlled->GetActorUpVector();
	FVector Forward = Controlled->GetActorForwardVector();
	FVector TargetUp = (TrackingLocation - Controlled->GetActorLocation());

	if (TargetUp.IsNearlyZero()) return;
	TargetUp.Normalize();


	FVector NeedVector = TargetUp - (FVector::DotProduct(TargetUp, Forward) * Forward);
	if (NeedVector.IsNearlyZero()) return;
	NeedVector.Normalize();

	float RollAmount = FVector::DotProduct(FVector::CrossProduct(CurrentUp, NeedVector), Forward);
	RollAmount = FMath::Clamp(RollAmount, -1.f, 1.f);

	BlackboardComp->SetValueAsFloat(RollKey.SelectedKeyName, RollAmount);

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
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("Pitch: %f"), PitchOffset));
	}
}