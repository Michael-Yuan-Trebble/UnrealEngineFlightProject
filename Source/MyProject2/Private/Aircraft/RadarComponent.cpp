// Fill out your copyright notice in the Description page of Project Settings.


#include "Aircraft/RadarComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Aircraft/BaseAircraft.h"
#include "AircraftRegistry.h"
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

	// Radar only triggers 0.5 seconds at a time
	GetWorld()->GetTimerManager().SetTimer(RadarScanTimer, this, &URadarComponent::ScanTargets, 0.5f, true);
}

void URadarComponent::Setup(ABaseAircraft* InControl) 
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
	// Gather aircraft from cached registry

	Enemies.Empty();
	if (!GetWorld()) return;
	AAircraftRegistry* Registry = AAircraftRegistry::Get(GetWorld());
	if (!Registry) return;

	// Only try to find enemies right now, later find allies and put a different reticle over them

	for (ABaseAircraft* RegisteredPawn : Registry->RegisteredAircraft) 
	{
		if (!IsValid(RegisteredPawn) || RegisteredPawn == Controlled) continue;

		if (RegisteredPawn->Faction == Controlled->Faction) continue;
		FDetectedAircraftInfo TempInfo;
		TempInfo.Location = RegisteredPawn->GetActorLocation();
		TempInfo.Rotation = RegisteredPawn->GetActorRotation();
		TempInfo.CurrentPawn = RegisteredPawn;
		TempInfo.threatLevel = TempInfo.CalculateThreat();


		if (TempInfo.threatLevel <= 0) continue;
		Enemies.Add(TempInfo);
		if (!Controlled) continue;
		if (!Controlled->GetController()->IsPlayerController()) continue;
		if (!Selected)
		{
			Selected = RegisteredPawn;
			Controlled->Tracking = Selected;
			FTimerHandle TempHandle;
		}
		if (HUD && !IsValid(HUD->SelectedAircraftWidget)) 
		{
			HUD->UpdateSelected(RegisteredPawn);
		}
	}
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
	Selected = NewTarget;
	if (!Controlled) return;
	Controlled->Tracking = NewTarget;
	// VFX
}