// Fill out your copyright notice in the Description page of Project Settings.

#include "Units/Components/Aircraft/RadarComponent.h"
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
	if (!IsValid(Controlled) || !GetWorld()) return;

	UAircraftRegistry* Registry = UAircraftRegistry::Get(GetWorld());
	if (!IsValid(Registry)) return;

	if (!Selected.IsValid() || Selected->IsPendingKillPending())
	{
		HandleSelectedDestroyed();
	}

	Enemies.Empty();

	ABaseUnit* Previous = Selected.Get();
	ABaseUnit* FirstSelected = nullptr;

	for (const TWeakObjectPtr<ABaseUnit>& RegisteredPawn : Registry->RegisteredUnits)
	{
		ABaseUnit* T = RegisteredPawn.Get();
		if (!IsValid(T) || T == Controlled) continue;
		if (T->GetFaction() == Controlled->GetFaction() || !T->IsLockable()) continue;

		FDetectedAircraftInfo Info;
		Info.CurrentPawn = T;
		Info.Location = T->GetActorLocation();
		Info.Rotation = T->GetActorRotation();
		Info.threatLevel = Info.CalculateThreat();

		if (Info.threatLevel <= 0) continue;
		Enemies.Add(Info);

		if (!IsValid(FirstSelected))
		{
			FirstSelected = T;
		}
	}

	if ((Selected.IsValid() || !Selected->IsValidLowLevel()) && IsValid(FirstSelected) && FirstSelected != Previous)
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

	if (IsValid(Controlled)) Controlled->SetTracking(nullptr);
	if (IsValid(HUD)) HUD->SetTarget(nullptr);

	CycleToNextTarget();
}

// ====================================
// Player Aircraft selection
// ====================================

void URadarComponent::CycleTarget() 
{
	if (Enemies.Num() == 0 || !IsValid(GetWorld())) return;

	FVector CameraLoc = FVector::ZeroVector;
	FRotator CameraRot = FRotator::ZeroRotator;

	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(CameraLoc, CameraRot);
	const FVector Forward = CameraRot.Vector();

	float BestDot = -1.0f;
	AActor* ClosestTarget = nullptr;

	for (const FDetectedAircraftInfo& Target : Enemies)
	{
		AActor* GetTarget = Target.CurrentPawn.Get();
		if (!IsValid(GetTarget)) return;

		// Find the distance between target and where the player's camera is pointing
		FVector ToTarget = (GetTarget->GetActorLocation() - CameraLoc).GetSafeNormal();
		float Dot = FVector::DotProduct(Forward, ToTarget);

		if (Dot > BestDot)
		{
			BestDot = Dot;
			ClosestTarget = GetTarget;
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
	if (Enemies.Num() <= 1 || !IsValid(GetWorld())) return;

	FVector CameraLoc = FVector::ZeroVector;
	FRotator CameraRot = FRotator::ZeroRotator;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(CameraLoc, CameraRot);
	const FVector Forward = CameraRot.Vector();

	float BestAngleDiff = FLT_MAX;
	AActor* BestTarget = nullptr;

	for (const FDetectedAircraftInfo& Target : Enemies)
	{
		AActor* TargetGet = Target.CurrentPawn.Get();
		if (!IsValid(TargetGet) || TargetGet == Selected.Get()) return;

		FVector ToTarget = (TargetGet->GetActorLocation() - CameraLoc).GetSafeNormal();
		float Dot = FMath::Clamp(FVector::DotProduct(Forward, ToTarget), -1.0f, 1.0f);
		float AngleDiff = FMath::Acos(Dot);

		if (AngleDiff < BestAngleDiff)
		{
			BestAngleDiff = AngleDiff;
			BestTarget = TargetGet;
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
	ABaseUnit* Target = Selected.Get();
	if (!IsValid(Unit) || Target == Unit) return;

	if (IsValid(Target))
	{
		Target->OnUnitDeath.RemoveDynamic(this, &URadarComponent::HandleSelectedDestroyed);
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