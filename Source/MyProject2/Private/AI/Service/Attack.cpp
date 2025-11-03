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
	FVector ToTargetWorld = (Selected->GetActorLocation() - Controlled->GetActorLocation()).GetSafeNormal();
	if (ToTargetWorld.IsNearlyZero()) return;

	FTransform AirframeTransform = Controlled->Airframe->GetComponentTransform();
	FVector LocalDir = AirframeTransform.InverseTransformVectorNoScale(ToTargetWorld);

	float YawErrorRad = FMath::Atan2(LocalDir.Y, LocalDir.X);
	float PitchErrorRad = FMath::Atan2(LocalDir.Z, LocalDir.X);

	float YawErrorDeg = FMath::RadiansToDegrees(YawErrorRad);
	float PitchErrorDeg = FMath::RadiansToDegrees(PitchErrorRad);

	float DesiredPitchInput = FMath::Clamp(PitchErrorDeg * PitchGain, -1.f, 1.f);
	float DesiredYawInput = FMath::Clamp(YawErrorDeg * YawGain, -1.f, 1.f);

	float DesiredRollInput = FMath::Clamp(-YawErrorDeg * RollGain, -1.f, 1.f);

	const float ErrorDeadzoneDeg = 1.0f;
	if (FMath::Abs(PitchErrorDeg) < ErrorDeadzoneDeg) DesiredPitchInput = 0.f;
	if (FMath::Abs(YawErrorDeg) < ErrorDeadzoneDeg) DesiredYawInput = 0.f;
	if (FMath::Abs(YawErrorDeg) < 0.5f)             DesiredRollInput = 0.f;

	GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Cyan,
		FString::Printf(TEXT("YawErr: %.2f PitchErr: %.2f"), DesiredPitchInput, DesiredRollInput));

	if (Controlled->FlightComponent)
	{
		BlackboardComp->SetValueAsFloat(RollKey.SelectedKeyName, -DesiredRollInput);
		BlackboardComp->SetValueAsFloat(PitchKey.SelectedKeyName, DesiredPitchInput);
		BlackboardComp->SetValueAsFloat(YawKey.SelectedKeyName, DesiredYawInput);
	}
}

// 180 to -180 degrees
float UBTServiceAttack::CalculateRollDegrees(float CurrentPitchErrorDeg, float CurrentYawErrorDeg)
{
	return 0;
}

float UBTServiceAttack::CalculatePitchDegrees() 
{
	return 0;
}

float UBTServiceAttack::CalculateYawDegrees()
{
	return 0;
}