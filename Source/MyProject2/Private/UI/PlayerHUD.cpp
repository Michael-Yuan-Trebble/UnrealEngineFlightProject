 // Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("HUD!"));
#include "UI/PlayerHUD.h"
#include "AircraftPlayerController.h"
#include "DrawDebugHelpers.h"
#include "Aircraft/BaseAircraft.h"
#include "UI/LockBoxWidget.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Aircraft/FlightComponent.h"
#include "Components/CanvasPanelSlot.h"
#include "Aircraft/WeaponSystemComponent.h"
#include "Aircraft/Player/PlayerAircraft.h"

APlayerHUD::APlayerHUD() 
{
    PrimaryActorTick.bCanEverTick = true;
	static ConstructorHelpers::FClassFinder<ULockBoxWidget> WidgetBPClass(TEXT("/Game/UI/LockBoxBP"));
	if (WidgetBPClass.Succeeded()) 
	{
		LockBoxWidgetClass = WidgetBPClass.Class;
	}
}

void APlayerHUD::BeginPlay()
{
    PC = Cast<AAircraftPlayerController>(GetOwningPlayerController());
    UpdateTargetWidgets();
    Super::BeginPlay();
    MiniMap = CreateWidget<UMinimapWidget>(GetWorld(), MiniMapClass);
    if (!MiniMap) return;
    MiniMap->AddToViewport();
    
    MiniMap->InitializeBounds(
        FVector2D(-20000.f, -20000.f),
        FVector2D(20000.f, 20000.f)
    );
}

void APlayerHUD::Init() 
{
    Controlled = Cast<APlayerAircraft>(PC->GetPawn());
    if (!Controlled) return;
    UWeaponSystemComponent* WeaponSys = Controlled->WeaponComponent;
    if (!WeaponSys) return;
    WeaponSys->OnWeaponCountUpdated.AddDynamic(this, &APlayerHUD::OnWeaponChanged);
    WeaponSys->GetCount();
    EquippedWeaponNames = WeaponSys->EquippedWeaponNames;

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

    // TODO: Pitch Ladder Visibility is true for now in testing, remove later because its only meant for certain modes, not always at start
    isPitchLadderVisible = true;
}

void APlayerHUD::OnWeaponChanged(FName WeaponName, int32 Current, int32 Max) 
{
    CurrentName = WeaponName;
    CurrentNum = Current;
    MaxNum = Max;
}

void APlayerHUD::Tick(float DeltaSeconds) 
{
	Super::Tick(DeltaSeconds);
    if (!LockBoxWidgetClass || !PC) return;
	UpdateTargetWidgets();

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

void APlayerHUD::PitchLadderUpdate() 
{
    if (!isPitchLadderVisible) return;
    float Pitch = FMath::RadiansToDegrees(FMath::Asin(Controlled->Airframe->GetForwardVector().Z));
    PitchLadderWidget->Update(Pitch);
}

void APlayerHUD::SetPitchLadderVisibility(bool isVisible) 
{
    isPitchLadderVisible = isVisible;
    PitchLadderWidget->SetVisibility(
        isPitchLadderVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden
    );
}

void APlayerHUD::UpdateLocked(bool Locked)
{
    if (!IsValid(SelectedAircraftWidget)) return;
    SelectedAircraftWidget->SetLockedOn(Locked);
}

void APlayerHUD::UpdateTargetWidgets() 
{
    for (auto It = ActiveWidgets.CreateIterator(); It; ++It)
    {
        ABaseUnit* Target = It.Key();
        if (!IsValid(Target) || !Targets.Contains(Target))
        {
            if (ULockBoxWidget* Reticle = It.Value())
            {
                Reticle->RemoveFromParent();
            }
            It.RemoveCurrent();
        }
    }

    for (ABaseUnit* Target : Targets)
    {
        if (!IsValid(Target)) continue;

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
        if (Target->Collision) 
        {
            WorldLocation = Target->Collision->GetComponentLocation();
        } 
        else
        {
            WorldLocation = Target->GetActorLocation();
        }

        if (!Reticle) continue;

        FVector2D ScreenPos;
        bool bProjected = PC->ProjectWorldLocationToScreen(WorldLocation, ScreenPos);

        FVector CameraLoc;
        FRotator CameraRot;
        PC->GetPlayerViewPoint(CameraLoc, CameraRot);

        FVector ToTarget = Target->GetActorLocation() - CameraLoc;
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

void APlayerHUD::UpdateSelected(ABaseUnit* In)
{
    if (ULockBoxWidget** FoundWidget = ActiveWidgets.Find(In))
    {
        SelectedAircraftWidget = *FoundWidget;
    }
    else
    {
        SelectedAircraftWidget = nullptr;
    }
}