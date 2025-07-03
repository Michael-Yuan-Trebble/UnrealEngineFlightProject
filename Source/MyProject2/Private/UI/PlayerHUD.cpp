// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PlayerHUD.h"
#include "AircraftPlayerController.h"
#include "DrawDebugHelpers.h"
#include "UI/LockBoxWidget.h"

APlayerHUD::APlayerHUD() 
{
	static ConstructorHelpers::FClassFinder<ULockBoxWidget> WidgetBPClass(TEXT("/Game/UI/MyLockBoxWidget"));
	if (WidgetBPClass.Succeeded()) 
	{
		LockBoxWidgetClass = WidgetBPClass.Class;
	}
}

void APlayerHUD::BeginPlay()
{
	Super::BeginPlay();
}

void APlayerHUD::Tick(float DeltaSeconds) 
{
	Super::Tick(DeltaSeconds);
	if (LockBoxWidgetClass && PC) 
	{
		UpdateTargetWidgets();
	}
}

void APlayerHUD::UpdateTargetWidgets() 
{
	const TArray<FDetectedAircraftInfo>& Targets = PC->GetDetectedTargets();
	FVector2D ViewportSize;
	GEngine->GameViewport->GetViewportSize(ViewportSize);
	FVector2D ScreenCenter = ViewportSize * 0.5f;

	for (const FDetectedAircraftInfo& Info : Targets) 
	{
		AActor* Target = Info.CurrentPawn;
		if (!ActiveWidgets.Contains(Target)) 
		{
			ULockBoxWidget* Widget = CreateWidget<ULockBoxWidget>(PC, LockBoxWidgetClass);
			Widget->AddToViewport();
			Widget->SetAlignmentInViewport(FVector2D(0.5f, 0.5f));
			ActiveWidgets.Add(Target, Widget);
			//Widget->SetPositionInViewport(ScreenCenter, false);
		}
		FVector WorldLocation = FVector::ZeroVector;

		if (UPrimitiveComponent* CollisionComp = Target->FindComponentByClass<UPrimitiveComponent>())
		{
			WorldLocation = CollisionComp->GetComponentLocation();
		}
		else
		{
			WorldLocation = Target->GetActorLocation();
		}
		FVector CameraLoc;
		FRotator CameraRot;
		PC->GetPlayerViewPoint(CameraLoc, CameraRot);

		FVector ToTarget = (WorldLocation - CameraLoc).GetSafeNormal();
		float Dot = FVector::DotProduct(CameraRot.Vector(), ToTarget);

		bool bIsInFront = Dot > 0.0f;

		FVector2D ScreenPos;

		DrawDebugSphere(GetWorld(), WorldLocation, 1.0f, 8, FColor::Red, false, 0.1f);

		if (PC->ProjectWorldLocationToScreen(WorldLocation, ScreenPos))
		{
			FVector2D& CachedScreenPos = TargetScreenPositions.FindOrAdd(Target);
			CachedScreenPos = FMath::Vector2DInterpTo(CachedScreenPos, ScreenPos, GetWorld()->GetDeltaSeconds(), 10.0f);
			if (bIsInFront)
			{
				ActiveWidgets[Target]->SetVisibility(ESlateVisibility::Visible);
				ActiveWidgets[Target]->SetPositionInViewport(ScreenPos, false);
			}
			else
			{
				ActiveWidgets[Target]->SetVisibility(ESlateVisibility::Hidden);
			}
		}
	}
}