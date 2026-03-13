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
}

void UBTServiceShootGun::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) 
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AAircraftAIController* Controller = Cast<AAircraftAIController>(OwnerComp.GetAIOwner());
	if (!Controller) return;

	AActor* Controlled = Controller->GetPawn();
	UBlackboardComponent* Comp = OwnerComp.GetBlackboardComponent();
	AActor* Selected = Cast<AActor>(Comp->GetValueAsObject(TargetActorKey.SelectedKeyName));

	bool bFire = IsInsideCone(Controlled, Selected);
	if (IsValid(Comp)) Comp->SetValueAsBool(bFireGun.SelectedKeyName, bFire);
}

bool UBTServiceShootGun::IsInsideCone(AActor* Controlled, AActor* Selected)
{
	if (!IsValid(Selected) || !IsValid(Controlled)) return false;

	FVector ToPlayer = (Selected->GetActorLocation() - Controlled->GetActorLocation()).GetSafeNormal();
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

	float Distance = FVector::Dist(Controlled->GetActorLocation(), Selected->GetActorLocation());

	return (CosAngle >= Threshold) && (Distance < ShootDistance);
}