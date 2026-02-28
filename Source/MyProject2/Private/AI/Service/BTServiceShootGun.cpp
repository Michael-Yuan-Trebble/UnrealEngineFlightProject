// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Service Fire Gun!"));
#include "AI/Service/BTServiceShootGun.h"
#include "DrawDebugHelpers.h"
#include "Units/Aircraft/WeaponSystemComponent.h"
#include "Units/Aircraft/FlightComponent.h"
#include "Units/Aircraft/AI/EnemyAircraft.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Units/Aircraft/AI/EnemyAircraftAI.h"

UBTServiceShootGun::UBTServiceShootGun() {
	NodeName = "Update Gun Shot";
	Interval = 2.f;
	bNotifyBecomeRelevant = true;
}

void UBTServiceShootGun::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) 
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);
	BlackboardComp = OwnerComp.GetBlackboardComponent();
	Controller = Cast<AEnemyAircraftAI>(OwnerComp.GetAIOwner());
	Selected = Cast<AActor>(BlackboardComp->GetValueAsObject(TargetActorKey.SelectedKeyName));
}

void UBTServiceShootGun::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) 
{
	if (!IsValid(Selected)) return;
	if (IsInsideCone())
	{
		BlackboardComp->SetValueAsBool(bFireGun.SelectedKeyName, true);
	}
	else
	{
		BlackboardComp->SetValueAsBool(bFireGun.SelectedKeyName, false);
	}
}

bool UBTServiceShootGun::IsInsideCone()
{
	if (!IsValid(Selected) || !IsValid(Controller)) return false;
	FVector ToPlayer = (Selected->GetActorLocation() - Controller->GetPawn()->GetActorLocation()).GetSafeNormal();
	FVector Forward = Controller->GetPawn()->GetActorForwardVector();

	float CosAngle = FVector::DotProduct(Forward, ToPlayer);
	float Threshold = FMath::Cos(FMath::DegreesToRadians(ConeAngle));

	if (APawn* Pawn = Controller->GetPawn())
	{
		float HalfAngleRad = FMath::DegreesToRadians(ConeAngle);

		/*
		DrawDebugCone(
			GetWorld(),
			Pawn->GetActorLocation(),
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
	}

	float Distance = FVector::Dist(Controller->GetPawn()->GetActorLocation(), Selected->GetActorLocation());

	return (CosAngle >= Threshold) && (Distance < ShootDistance);
}