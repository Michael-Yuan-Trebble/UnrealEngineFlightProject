// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Task Fire Gun!"));
#define printFalse(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Yellow, TEXT("Not Firing!"));
#include "AI/Task/BTTaskFireGun.h"
#include "Aircraft/AI/EnemyAircraftAI.h"

UBTTaskFireGun::UBTTaskFireGun() 
{
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTaskFireGun::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) 
{
	BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp) return EBTNodeResult::Aborted;

	Controller = Cast<AEnemyAircraftAI>(OwnerComp.GetAIOwner());
	if (!Controller) return EBTNodeResult::Aborted;

	Controller->SetFlying(true);
	Selected = Cast<AActor>(BlackboardComp->GetValueAsObject(TargetActorKey.SelectedKeyName));
	return EBTNodeResult::InProgress;
}

void UBTTaskFireGun::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) 
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	if (!IsValid(Selected)) return;
	if (IsInsideCone(Selected, 8.0f) && CanShotHit(Selected)) 
	{
		Controller->ShootGun(true);
		print()
	}
	else 
	{
		printFalse()
		Controller->ShootGun(false);
	}
}


bool UBTTaskFireGun::IsInsideCone(AActor* Player, float ConeDegrees) 
{
	FVector ToPlayer = (Player->GetActorLocation() - Controller->GetPawn()->GetActorLocation()).GetSafeNormal();
	FVector Forward = Controller->GetPawn()->GetActorForwardVector();

	float CosAngle = FVector::DotProduct(Forward, ToPlayer);
	float Threshold = FMath::Cos(FMath::DegreesToRadians(ConeDegrees));

	return CosAngle >= Threshold;
}
bool UBTTaskFireGun::CanShotHit(AActor* Player) 
{
	float BulletSpeed = 1500.0f;
	float Gravity = 980.0f;

	FVector Muzzle = Controller->GetPawn()->GetActorLocation();
	FVector Target = Player->GetActorLocation();

	float Distance = FVector::Distance(Muzzle, Target);
	float Time = Distance / BulletSpeed;

	float Drop = 0.5f * Gravity * Time * Time;

	float TargetHeightDifference = Muzzle.Z - Target.Z;

	return FMath::Abs(TargetHeightDifference - Drop) < 300;
}