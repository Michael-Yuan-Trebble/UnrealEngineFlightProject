// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Radar!"));
#include "Units/Aircraft/RadarComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Units/BaseUnit.h"
#include "Subsystem/AircraftRegistry.h"
#include "Engine/World.h"
#include "UI/PlayerHUD.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/SpectatorPawn.h"

URadarComponent::URadarComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void URadarComponent::BeginPlay()
{
	Super::BeginPlay();

	const float ScanTime = 1.f;
	GetWorld()->GetTimerManager().SetTimer(RadarScanTimer, this, &URadarComponent::ScanTargets, ScanTime, true);
}

void URadarComponent::Setup(ABaseUnit* InControl)
{
	Controlled = InControl;
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC) return;
	HUD = Cast<APlayerHUD>(PC->GetHUD());
}

void URadarComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void URadarComponent::ScanTargets()
{
	if (!Controlled || !GetWorld()) return;

	UAircraftRegistry* Registry = UAircraftRegistry::Get(GetWorld());
	if (!Registry) return;

	if (!IsValid(Selected) || Selected->IsPendingKillPending())
	{
		HandleSelectedDestroyed();
	}

	Enemies.Empty();

	ABaseUnit* Previous = Selected;

	ABaseUnit* FirstSelected = nullptr;
	for (TWeakObjectPtr<ABaseUnit> RegisteredPawn : Registry->RegisteredUnits)
	{
		if (!RegisteredPawn.IsValid()) continue;
		if (RegisteredPawn == Controlled) continue;
		if (RegisteredPawn->GetFaction() == Controlled->GetFaction()) continue;
		if (!RegisteredPawn->IsLockable()) continue;

		ABaseUnit* Unit = RegisteredPawn.Get();

		FDetectedAircraftInfo Info;
		Info.CurrentPawn = Unit;
		Info.Location = RegisteredPawn->GetActorLocation();
		Info.Rotation = RegisteredPawn->GetActorRotation();
		Info.threatLevel = Info.CalculateThreat();

		if (Info.threatLevel <= 0) continue;
		Enemies.Add(Info);

		if (!IsValid(FirstSelected))
		{
			FirstSelected = Unit;
		}
	}

	if ((!IsValid(Selected) || !Selected->IsValidLowLevel()) && IsValid(FirstSelected) && FirstSelected != Previous)
	{
		SetTarget(FirstSelected);
	}

	if (HUD)
	{
		RadarScanEvent.Broadcast(Enemies);
	}
}

void URadarComponent::HandleSelectedDestroyed() 
{
	Selected = nullptr;

	if (Controlled) Controlled->SetTracking(nullptr);
	if (HUD) HUD->SetTarget(nullptr);

	CycleToNextTarget();
}

// ====================================
// Player Aircraft selection
// ====================================

void URadarComponent::CycleTarget() 
{
	if (Enemies.Num() == 0) return;

	FVector CameraLoc;
	FRotator CameraRot;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(CameraLoc, CameraRot);
	const FVector Forward = CameraRot.Vector();

	float BestDot = -1.0f;
	AActor* ClosestTarget = nullptr;

	for (const FDetectedAircraftInfo Target : Enemies)
	{
		if (!IsValid(Target.CurrentPawn)) continue;

		// Find the distance between target and where the player's camera is pointing
		FVector ToTarget = (Target.CurrentPawn->GetActorLocation() - CameraLoc).GetSafeNormal();
		float Dot = FVector::DotProduct(Forward, ToTarget);

		if (Dot > BestDot)
		{
			BestDot = Dot;
			ClosestTarget = Target.CurrentPawn;
		}
	}

	if (ClosestTarget != Selected && ClosestTarget)
	{
		SetTarget(ClosestTarget);
	}
	else
	{
		CycleToNextTarget();
	}
}

void URadarComponent::CycleToNextTarget() 
{
	if (Enemies.Num() <= 1) return;

	FVector CameraLoc;
	FRotator CameraRot;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(CameraLoc, CameraRot);
	FVector Forward = CameraRot.Vector();

	float BestAngleDiff = FLT_MAX;
	AActor* BestTarget = nullptr;

	for (const FDetectedAircraftInfo Target : Enemies)
	{
		if (!IsValid(Target.CurrentPawn) || Target.CurrentPawn == Selected) continue;
		AActor* temp = Cast<AActor>(Target.CurrentPawn);

		FVector ToTarget = (temp->GetActorLocation() - CameraLoc).GetSafeNormal();
		float Dot = FMath::Clamp(FVector::DotProduct(Forward, ToTarget), -1.0f, 1.0f);
		float AngleDiff = FMath::Acos(Dot);

		if (AngleDiff < BestAngleDiff)
		{
			BestAngleDiff = AngleDiff;
			BestTarget = temp;
		}
	}

	if (BestTarget)
	{
		SetTarget(BestTarget);
	}
}

void URadarComponent::SetTarget(AActor* NewTarget) 
{
	ABaseUnit* Unit = Cast<ABaseUnit>(NewTarget);
	if (!IsValid(Unit) || Selected == Unit) return;

	if (IsValid(LastSelected) && Unit->GetActorGuid() == LastSelected->GetActorGuid()) return;

	if (IsValid(Selected))
	{
		Selected->OnUnitDeath.RemoveDynamic(this, &URadarComponent::HandleSelectedDestroyed);
	}

	Selected = Unit;
	if (!Unit->OnUnitDeath.IsAlreadyBound(this, &URadarComponent::HandleSelectedDestroyed))
	{
		Unit->OnUnitDeath.AddDynamic(this, &URadarComponent::HandleSelectedDestroyed);
	}

	if (HUD) HUD->SetTarget(Unit);
	LastSelected = Unit;
	if (Controlled) Controlled->SetTracking(Unit);
	// VFX
}