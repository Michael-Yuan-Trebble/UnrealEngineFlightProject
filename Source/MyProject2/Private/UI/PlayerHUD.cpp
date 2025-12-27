 // Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("HUD!"));
#include "UI/PlayerHUD.h"
#include "Player Info/AircraftPlayerController.h"
#include "DrawDebugHelpers.h"
#include "Units/Aircraft/BaseAircraft.h"
#include "UI/LockBoxWidget.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Units/Aircraft/FlightComponent.h"
#include "Units/Aircraft/RadarComponent.h"
#include "Components/CanvasPanelSlot.h"
#include "Units/Aircraft/WeaponSystemComponent.h"
#include "Units/Aircraft/Player/PlayerAircraft.h"

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

void APlayerHUD::UpdateLocked(float LockPercent)
{
    if (!IsValid(Target)) return;
    ULockBoxWidget** FoundWidget = ActiveWidgets.Find(Target);
    if (FoundWidget && IsValid(*FoundWidget))
    {
        ULockBoxWidget* Widget = *FoundWidget;
        if (Widget) 
        {
            Widget->UpdateLockProgress(LockPercent);
        }
    }
}

void APlayerHUD::UpdateTargetWidgets()
{
    if (!LockBoxWidgetClass) return;

    for (auto It = ActiveWidgets.CreateIterator(); It; ++It)
    {
        ABaseUnit* Actor = It.Key();
        ULockBoxWidget* Widget = It.Value();

        if (!IsValid(Actor) || !Targets.Contains(Actor))
        {
            if (Widget) Widget->RemoveFromParent();
            It.RemoveCurrent();
        }
    }

    for (ABaseUnit* TargetActor : Targets)
    {
        if (!IsValid(TargetActor)) continue;

        ULockBoxWidget* Reticle = ActiveWidgets.FindRef(TargetActor);
        if (!Reticle)
        {
            Reticle = CreateWidget<ULockBoxWidget>(PC, LockBoxWidgetClass);
            if (Reticle)
            {
                Reticle->AddToViewport();
                ActiveWidgets.Add(TargetActor, Reticle);
            }
        }
    }

    for (auto& Pair : ActiveWidgets)
    {
        ABaseUnit* Actor = Pair.Key;
        ULockBoxWidget* Reticle = Pair.Value;
        if (!IsValid(Actor) || !Reticle) continue;

        FVector WorldLocation = Actor->GetActorLocation();
        FVector2D ScreenPos;
        bool bProjected = PC->ProjectWorldLocationToScreen(WorldLocation, ScreenPos);

        FVector CameraLoc;
        FRotator CameraRot;
        PC->GetPlayerViewPoint(CameraLoc, CameraRot);

        bool bInFront = FVector::DotProduct((WorldLocation - CameraLoc), CameraRot.Vector()) > 0;
        bool bVisible = bProjected && bInFront;

        Reticle->SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);

        if (bVisible)
        {
            FVector2D WidgetSize = Reticle->GetDesiredSize();
            Reticle->SetPositionInViewport(ScreenPos - WidgetSize * 0.5f, true);
        }

    }

    UpdateSelected();
}

void APlayerHUD::HandleRadarScan(const TArray<FDetectedAircraftInfo>& InEnemies)
{
    TArray<ABaseUnit*> Array;
    if (!Controlled) return;
    for (FDetectedAircraftInfo T : InEnemies) 
    {
        ABaseUnit* Unit = Cast<ABaseUnit>(T.CurrentPawn);
        if (!Unit) continue;
        if (Unit->Faction == Controlled->Faction) continue;
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
    Controlled->WeaponComponentOnUnitDeath();
}

void APlayerHUD::UpdateSelected()
{
    if (!IsValid(Target))
    {
        SelectedAircraftWidget = nullptr;
        LastActor = nullptr;
        return;
    }

    if (LastActor == Target) return;

    LastActor = Target;

    ULockBoxWidget** FoundWidget = ActiveWidgets.Find(Target);

    if (FoundWidget && IsValid(*FoundWidget))
    {
        SelectedAircraftWidget = *FoundWidget;
        SelectedAircraftWidget->SelectedAnimation();
    }
    else
    {
        SelectedAircraftWidget = nullptr;
    }
}

void APlayerHUD::SetTarget(TWeakObjectPtr<ABaseUnit> InTarget)
{
    if (!InTarget.IsValid()) return;
    APlayerAircraft* IfControl = Cast<APlayerAircraft>(InTarget.Get());
    if (IfControl) return;

    if (Target) 
    {
        ULockBoxWidget** FoundWidget = ActiveWidgets.Find(Target);
        if (FoundWidget && IsValid(*FoundWidget))
        {
            ULockBoxWidget* Temp = *FoundWidget;
            Temp->SelectStop();
        }
    }

    Target = InTarget.Get(); 
    if (!Targets.Contains(Target)) Targets.Add(Target); 
    UpdateTargetWidgets();
}

void APlayerHUD::HandleMissileLaunchedAtSelf(ABaseMissile* Missile) 
{

}

void APlayerHUD::HandleMissileLockedAtSelf() 
{

}

void APlayerHUD::EndPlay(const EEndPlayReason::Type EndPlayReason) 
{
    for (auto& Pair : ActiveWidgets)
    {
        if (Pair.Value) Pair.Value->RemoveFromParent();
    }
    ActiveWidgets.Empty();

    Super::EndPlay(EndPlayReason);
}