// Fill out your copyright notice in the Description page of Project Settings.


#include "LockBoxWidget.h"
#include "Engine/Texture2D.h"
#include "Components/SizeBox.h"
#include "Components/Image.h"

void ULockBoxWidget::SetLockedOn(bool isLocked) 
{
	isLockedOn = isLocked;
	if (ReticleImage) 
	{
		ReticleImage->SetColorAndOpacity(isLocked ? FLinearColor::Green : FLinearColor::White);
	}
}

void ULockBoxWidget::SetReticleImage(UTexture2D* NewTexture) {
	if (ReticleImage && NewTexture) {
		ReticleImage->SetBrushFromTexture(NewTexture,true);
	}
}