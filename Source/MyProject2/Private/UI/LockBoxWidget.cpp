// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Lock!"));
#include "UI/LockBoxWidget.h"
#include "Engine/Texture2D.h"
#include "Components/SizeBox.h"
#include "Styling/SlateBrush.h"
#include "Components/Image.h"

void ULockBoxWidget::SetLockedOn(bool isLocked) 
{
	if (isLockedOn == isLocked) return;
	isLockedOn = isLocked;
	if (ReticleImage) 
	{
		// TODO: Change from opacity to color when reticle texture changes
		FLinearColor Color = ReticleImage->ColorAndOpacity;
		Color.A = !isLocked ? 1 : 0.1f;
		ReticleImage->SetColorAndOpacity(Color);
	}
}

void ULockBoxWidget::SetReticleImage(UTexture2D* NewTexture) 
{
	if (ReticleImage && NewTexture) 
	{
		FSlateBrush Brush;
		Brush.SetResourceObject(NewTexture);
		Brush.ImageSize = FVector2D(128, 128);
		Brush.DrawAs = ESlateBrushDrawType::Image;
		ReticleImage->SetBrush(Brush);
	}
}