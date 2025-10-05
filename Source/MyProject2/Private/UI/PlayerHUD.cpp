 // Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("HUD!"));
#include "UI/PlayerHUD.h"
#include "AircraftPlayerController.h"
#include "DrawDebugHelpers.h"
#include "Aircraft/BaseAircraft.h"
#include "UI/LockBoxWidget.h"

APlayerHUD::APlayerHUD() 
{
	static ConstructorHelpers::FClassFinder<ULockBoxWidget> WidgetBPClass(TEXT("/Game/UI/LockBoxBP"));
	if (WidgetBPClass.Succeeded()) 
	{
		LockBoxWidgetClass = WidgetBPClass.Class;
	}
}

void APlayerHUD::BeginPlay()
{
	Super::BeginPlay();
    PC = Cast<AAircraftPlayerController>(GetOwningPlayerController());
    UpdateTargetWidgets();
}

void APlayerHUD::Tick(float DeltaSeconds) 
{
	Super::Tick(DeltaSeconds);
    if (!LockBoxWidgetClass || !PC) return;
	UpdateTargetWidgets();
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
        ABaseAircraft* Target = It.Key();
        if (!IsValid(Target) || !Targets.Contains(Target))
        {
            if (ULockBoxWidget* Reticle = It.Value())
            {
                Reticle->RemoveFromParent();
            }
            It.RemoveCurrent();
        }
    }

    for (ABaseAircraft* Target : Targets)
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
        if (Target->BodyCollision) 
        {
            WorldLocation = Target->BodyCollision->GetComponentLocation();
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

void APlayerHUD::UpdateSelected(ABaseAircraft* In) 
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