// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/Task/BTTaskFireGun.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AI/AircraftAIController.h"
#include "Debug/DebugHelper.h"

UBTTaskFireGun::UBTTaskFireGun() 
{
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTaskFireGun::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) 
{
	return EBTNodeResult::InProgress;
}

void UBTTaskFireGun::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) 
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	AAircraftAIController* Controller = Cast<AAircraftAIController>(OwnerComp.GetAIOwner());
	if (!Controller) return;

	AActor* Controlled = Controller->GetPawn();
	UBlackboardComponent* Comp = OwnerComp.GetBlackboardComponent();
	AActor* Selected = Cast<AActor>(Comp->GetValueAsObject(TargetActorKey.SelectedKeyName));

	if (IsInsideCone(Controlled, Selected) && CanShotHit(Controlled, Selected) && IsValid(Controller)) 
	{
		Controller->ShootGun(true);
		AIR_DEBUG_KEY(1, FColor::Red, "FIRING");
	}
	else 
	{
		AIR_DEBUG_KEY(1, FColor::Red, "NOT FIRING");
		Controller->ShootGun(false);
	}
}


bool UBTTaskFireGun::IsInsideCone(AActor* Self, AActor* Target) 
{
	if (!IsValid(Self) || !IsValid(Target)) return false;
	FVector ToPlayer = (Target->GetActorLocation() - Self->GetActorLocation()).GetSafeNormal();
	FVector Forward = Self->GetActorForwardVector();

	float CosAngle = FVector::DotProduct(Forward, ToPlayer);
	float Threshold = FMath::Cos(FMath::DegreesToRadians(ConeDegrees));

	return CosAngle >= Threshold;
}
bool UBTTaskFireGun::CanShotHit(AActor* Target, AActor* Self) 
{
	if (!IsValid(Self) || !IsValid(Target)) return false;
	float BulletSpeed = 1500.0f;
	float Gravity = 980.0f;

	FVector Muzzle = Self->GetActorLocation();
	FVector TargetVector = Target->GetActorLocation();

	float Distance = FVector::Distance(Muzzle, TargetVector);
	float Time = Distance / BulletSpeed;

	float Drop = 0.5f * Gravity * Time * Time;

	float TargetHeightDifference = Muzzle.Z - TargetVector.Z;

	return FMath::Abs(TargetHeightDifference - Drop) < 300;
}