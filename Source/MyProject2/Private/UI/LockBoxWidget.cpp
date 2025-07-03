// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/LockBoxWidget.h"
#include "Engine/Texture2D.h"
#include "Components/SizeBox.h"
#include "Styling/SlateBrush.h"
#include "Components/Image.h"

void ULockBoxWidget::SetLockedOn(bool isLocked) 
{
	isLockedOn = isLocked;
	if (ReticleImage) 
	{
		ReticleImage->SetColorAndOpacity(FLinearColor::Green);
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