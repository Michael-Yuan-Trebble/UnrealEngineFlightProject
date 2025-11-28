 // Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("HUD!"));
#include "UI/PlayerHUD.h"
#include "AircraftPlayerController.h"
#include "DrawDebugHelpers.h"
#include "Aircraft/BaseAircraft.h"
#include "UI/LockBoxWidget.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Aircraft/FlightComponent.h"
#include "Aircraft/RadarComponent.h"
#include "Components/CanvasPanelSlot.h"
#include "Aircraft/WeaponSystemComponent.h"
#include "Aircraft/Player/PlayerAircraft.h"

APlayerHUD::APlayerHUD() 
{
    PrimaryActorTick.bCanEverTick = true;
}

void APlayerHUD::BeginPlay()
{
    Super::BeginPlay();
}

void APlayerHUD::Init(AAircraftPlayerController* InPC) 
{
    PC = InPC;
    //UpdateTargetWidgets();
    MiniMap = CreateWidget<UMinimapWidget>(GetWorld(), MiniMapClass);
    if (!MiniMap) return;
    MiniMap->AddToViewport();

    MiniMap->InitializeBounds(
        FVector2D(-20000.f, -20000.f),
        FVector2D(20000.f, 20000.f)
    );

    if (!AimReticleClass) return;
    AimReticleWidget = CreateWidget<UUserWidget>(PC, AimReticleClass);
    AimReticleWidget->AddToViewport();
    AimReticleWidget->SetAlignmentInViewport(FVector2D(0.5f, 0.5f));

    if (!AOAReticleClass) return;
    AOAReticleWidget = CreateWidget<UUserWidget>(PC, AOAReticleClass);
    AOAReticleWidget->AddToViewport();
    AOAReticleWidget->SetAlignmentInViewport(FVector2D(0.5f, 0.5f));

    if (!PitchLadderClass) return;
    PitchLadderWidget = CreateWidget<UPitchLadder>(PC, PitchLadderClass);
    PitchLadderWidget->AddToViewport();
    PitchLadderWidget->SetAlignmentInViewport(FVector2D(0.5f, 0.5f));

    if (!HitNotiClass) return;
    HitNotiWidget = CreateWidget<UHitNotificationWidget>(PC, HitNotiClass);
    HitNotiWidget->AddToViewport();
    HitNotiWidget->HideMessage();
    HitNotiWidget->SetAlignmentInViewport(FVector2D(0.5f, 0.5f));

    Controlled = Cast<APlayerAircraft>(PC->GetPawn());
    if (!Controlled) return;
    URadarComponent* Radar = Controlled->RadarComponent;
    if (!Radar) return;;
    Radar->RadarScanEvent.AddDynamic(this, &APlayerHUD::HandleRadarScan);
}

void APlayerHUD::OnWeaponChanged(FName WeaponName, int32 Current, int32 Max) 
{
    CurrentName = WeaponName;
    CurrentNum = Current;
    MaxNum = Max;
}

void APlayerHUD::HandleWeaponMiss()
{
    if (!HitNotiWidget) return;
    HitNotiWidget->ShowMessage(FText::FromString("Missed"));
}

void APlayerHUD::UpdateTargetHit(bool bDestroyed) 
{
    if (!HitNotiWidget) return;
    if (bDestroyed) {
        HitNotiWidget->ShowMessage(FText::FromString("Destroyed"));
    }
    else {
        HitNotiWidget->ShowMessage(FText::FromString("Hit"));
    }
}

void APlayerHUD::Tick(float DeltaSeconds) 
{
	Super::Tick(DeltaSeconds);
    if (!PC) return;
    if (LockBoxWidgetClass) 
    {
        UpdateTargetWidgets();
    }

    if (!Controlled || !AimReticleWidget || !AOAReticleWidget) return;
    FVector CamLoc;
    FRotator CamRot;
    PC->GetPlayerViewPoint(CamLoc, CamRot);

    FVector NoseDir = Controlled->Airframe->GetForwardVector();
    FVector ForwardDir = Controlled->GetActorForwardVector();

    FVector AimWorldPos = CamLoc + NoseDir * 10000.f;

    FVector AOAWorldPos = CamLoc + ForwardDir * 10000.f;

    FVector2D ScreenPos;
    if (PC->ProjectWorldLocationToScreen(AimWorldPos, ScreenPos, true))
    {
        AimReticleWidget->SetAlignmentInViewport(FVector2D(0.5f, 0.5f));
        AimReticleWidget->SetPositionInViewport(ScreenPos, true);
    }
    if (PC->ProjectWorldLocationToScreen(AOAWorldPos, ScreenPos, true))
    {
        AOAReticleWidget->SetAlignmentInViewport(FVector2D(0.5f, 0.5f));
        AOAReticleWidget->SetPositionInViewport(ScreenPos, true);
    }
    if (!PitchLadderWidget) return;

    FVector LadderWorldPos = CamLoc + NoseDir * 10000.f;
    FVector2D LadderScreenPos;

    if (PC->ProjectWorldLocationToScreen(LadderWorldPos, LadderScreenPos, true))
    {
        PitchLadderWidget->SetPositionInViewport(LadderScreenPos, true);
    }

    float Pitch = FMath::RadiansToDegrees(FMath::Asin(Controlled->Airframe->GetForwardVector().Z));
    PitchLadderWidget->Update(Pitch);
}

void APlayerHUD::TogglePitchLadder(bool Toggle) {
    if (!IsValid(PitchLadderWidget) || !PitchLadderWidget) return;

    PitchLadderWidget->SetVisibility(Toggle ? ESlateVisibility::Hidden : ESlateVisibility::Visible);
    isPitchLadderVisible = !Toggle;
}

void APlayerHUD::PitchLadderUpdate() 
{
    if (!isPitchLadderVisible) return;
    float Pitch = FMath::RadiansToDegrees(FMath::Asin(Controlled->Airframe->GetForwardVector().Z));
    PitchLadderWidget->Update(Pitch);
}

void APlayerHUD::UpdateLocked(bool Locked)
{
    if (!IsValid(SelectedAircraftWidget)) return;
    SelectedAircraftWidget->SetLockedOn(Locked);
}

void APlayerHUD::UpdateTargetWidgets() 
{
    if (!LockBoxWidgetClass) return;

    for (auto It = ActiveWidgets.CreateIterator(); It; ++It)
    {
        ABaseUnit* Target = It.Key();
        if (!IsValid(Target) || Target->IsActorBeingDestroyed() || !Targets.Contains(Target))
        {
            if (ULockBoxWidget* Reticle = It.Value())
            {
                Reticle->RemoveFromParent();
            }
            It.RemoveCurrent();
        }
    }

    for (int32 i = Targets.Num() - 1; i >= 0; i--)
    {
        ABaseUnit* Target = Targets[i];

        if (!IsValid(Target) || Target->IsActorBeingDestroyed())
        {
            Targets.RemoveAt(i);
            continue;
        }

        ULockBoxWidget* Reticle = ActiveWidgets.FindRef(Target);
        if (!Reticle)
        {
            Reticle = CreateWidget<ULockBoxWidget>(GetWorld(), LockBoxWidgetClass);
            if (Reticle)
            {
                Reticle->AddToViewport();
                ActiveWidgets.Add(Target, Reticle);
            }
        }

        FVector WorldLocation;
        if (!IsValid(Target) || Target->IsActorBeingDestroyed()) continue;

        USceneComponent* Comp = Target->GetRootComponent();
        if (!IsValid(Comp)) continue;

        WorldLocation = Comp->GetComponentLocation();

        if (!Reticle) continue;

        FVector2D ScreenPos;
        bool bProjected = PC->ProjectWorldLocationToScreen(WorldLocation, ScreenPos);

        FVector CameraLoc;
        FRotator CameraRot;
        PC->GetPlayerViewPoint(CameraLoc, CameraRot);

        FVector Location = Comp->GetComponentLocation();
        FVector ToTarget = Location - CameraLoc;
        bool bInFront = FVector::DotProduct(CameraRot.Vector(), ToTarget) > 0;

        if (bProjected && bInFront)
        {
            FVector2D WidgetSize = Reticle->GetDesiredSize();
            FVector2D CenteredPos = ScreenPos - (WidgetSize * 0.5f);
            Reticle->SetPositionInViewport(CenteredPos, true);
            Reticle->SetVisibility(ESlateVisibility::Visible);
        }
        else
        {
            Reticle->SetVisibility(ESlateVisibility::Hidden);
        }
    }
}

void APlayerHUD::HandleRadarScan(const TArray<FDetectedAircraftInfo>& InEnemies) {
    TArray<ABaseUnit*> Array;
    for (FDetectedAircraftInfo T : InEnemies) 
    {
        ABaseUnit* Unit = Cast<ABaseUnit>(T.CurrentPawn);
        if (!Unit) continue;
        Array.Add(Unit);
    }
    Targets = Array;
}

void APlayerHUD::OnUnitDestroyed(ABaseUnit* Death)
{
    Targets.Remove(Death);
    if (ULockBoxWidget* W = ActiveWidgets.FindRef(Death)) 
    {
        W->RemoveFromParent();
    }

    ActiveWidgets.Remove(Death);
}

void APlayerHUD::UpdateSelected(ABaseUnit* In)
{
    if (!IsValid(In))
    {
        SelectedAircraftWidget = nullptr;
        LastActor = nullptr;
        return;
    }

    if (LastActor && LastActor->GetUniqueID() == In->GetUniqueID())
        return;

    LastActor = In;

    if (ULockBoxWidget** FoundWidget = ActiveWidgets.Find(In))
    {
        print(text)
            SelectedAircraftWidget = *FoundWidget;
    }
    else
    {
        SelectedAircraftWidget = nullptr;
    }
}