// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/Service/BTServiceShootGun.h"
#include "DrawDebugHelpers.h"
#include "AI/AircraftAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTServiceShootGun::UBTServiceShootGun() {
	NodeName = "Update Gun Shot";
	Interval = 2.f;
	bNotifyBecomeRelevant = true;
	bNotifyTick = true;
}

void UBTServiceShootGun::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) 
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);
	BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!IsValid(BlackboardComp)) return;
	AAircraftAIController* Controller = Cast<AAircraftAIController>(OwnerComp.GetAIOwner());
	Controlled = Controller->GetPawn();
	Selected = Cast<AActor>(BlackboardComp->GetValueAsObject(TargetActorKey.SelectedKeyName));
}

void UBTServiceShootGun::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) 
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	bool bFire = IsInsideCone();
	if (IsValid(BlackboardComp)) BlackboardComp->SetValueAsBool(bFireGun.SelectedKeyName, bFire);
}

bool UBTServiceShootGun::IsInsideCone()
{
	AActor* Target = Selected.Get();
	if (!IsValid(Target) || !IsValid(Controlled)) return false;

	FVector ToPlayer = (Target->GetActorLocation() - Controlled->GetActorLocation()).GetSafeNormal();
	FVector Forward = Controlled->GetActorForwardVector();

	float CosAngle = FVector::DotProduct(Forward, ToPlayer);
	float Threshold = FMath::Cos(FMath::DegreesToRadians(ConeAngle));

	float HalfAngleRad = FMath::DegreesToRadians(ConeAngle);

		/*
		DrawDebugCone(
			GetWorld(),
			Controlled->GetActorLocation(),
			Forward,
			SHOOTDISTANCE,
			HalfAngleRad,
			HalfAngleRad,
			12,
			FColor::Red,
			false,
			0.05f,
			0,
			1.0f
		);*/

	float Distance = FVector::Dist(Controlled->GetActorLocation(), Target->GetActorLocation());

	return (CosAngle >= Threshold) && (Distance < ShootDistance);
}