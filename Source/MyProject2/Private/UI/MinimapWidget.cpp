// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MinimapWidget.h"
#include "Components/Image.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanelSlot.h"

void UMinimapWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if (!MapImage || !PlayerIcon) return;

	AActor* Player = UGameplayStatics::GetPlayerPawn(this, 0);
	if (!Player) return;

	FVector WorldLocation = Player->GetActorLocation();

	FVector2D MapPosition = WorldToMap(WorldLocation);

	FVector2D MapSize = MapImage->GetDesiredSize();

	FVector2D PixelPosition = FVector2D(MapPosition.X * MapSize.X, MapPosition.Y * MapSize.Y);

	PixelPosition -= PlayerIcon->GetDesiredSize() * 0.5f;

	UWidgetLayoutLibrary::SlotAsCanvasSlot(PlayerIcon)->SetPosition(PixelPosition);

	float Yaw = Player->GetActorRotation().Yaw;
	PlayerIcon->SetRenderTransformAngle(-Yaw);
}

void UMinimapWidget::InitializeBounds(const FVector2D& InWorldMin, const FVector2D& InWorldMax) 
{
	WorldMin = InWorldMin;
	WorldMax = InWorldMax;
}

FVector2D UMinimapWidget::WorldToMap(const FVector& WorldLocation) const 
{
	FVector2D WorldLocation2D = FVector2D(WorldLocation.X, WorldLocation.Y);

	FVector2D Normalized;
	Normalized.X = (WorldLocation2D.X - WorldMin.X) / (WorldMax.X - WorldMin.X);
	Normalized.Y = (WorldLocation2D.Y - WorldMin.Y) / (WorldMax.Y - WorldMin.Y);

	Normalized.X = FMath::Clamp(Normalized.X, 0.f, 1.f);
	Normalized.Y = FMath::Clamp(Normalized.Y, 0.f, 1.f);
	return Normalized;
}