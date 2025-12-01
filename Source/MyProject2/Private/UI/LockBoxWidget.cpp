// Fill out your copyright notice in the Description page of Project Settings.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Lock!"));
#include "UI/LockBoxWidget.h"
#include "Engine/Texture2D.h"
#include "Components/SizeBox.h"
#include "Styling/SlateBrush.h"
#include "Components/Image.h"

void ULockBoxWidget::NativeConstruct() 
{
	Super::NativeConstruct();

	if (SmallReticleImage)
	{
		FLinearColor Color = SmallReticleImage->GetColorAndOpacity();
		Color.A = 0.f;
		SmallReticleImage->SetColorAndOpacity(Color);
	}
}

void ULockBoxWidget::SetLockedOn(bool isLocked) 
{
	if (isLockedOn == isLocked) return;
	isLockedOn = isLocked;
	if (ReticleImage) 
	{
		
		// TODO: Change from opacity to color when reticle texture changes
		FLinearColor Color = ReticleImage->GetColorAndOpacity();
		Color.A = !isLocked ? 1 : 0.1f;
		ReticleImage->SetColorAndOpacity(Color);
	}
	if (SmallReticleImage) 
	{
		FLinearColor Color = SmallReticleImage->GetColorAndOpacity();
		Color.A = !isLocked ? 0.f : 0.1f;
		SmallReticleImage->SetColorAndOpacity(Color);
	}
}

void ULockBoxWidget::UpdateLockProgress(float Percent) 
{
	if (Percent <= 0.f)
	{
		ResetLockAnimation();
		return;
	}

	if (Percent >= 1.f)
	{
		if (!isLockedOn) 
		{
			isLockedOn = true;
			PlayFullLockAnimation();
		}
		return;
	}

	if (!bIsLocking) 
	{
		bIsLocking = true;
		PlayStartLockAnimation();
	}
}

void ULockBoxWidget::ResetLockAnimation()
{
	bIsLocking = false;
	isLockedOn = false;
	if (LockApproachAnim) StopAnimation(LockApproachAnim);
	if (LockConfirm) StopAnimation(LockConfirm);
	if (ReticleImage)
	{
		FLinearColor C = ReticleImage->GetColorAndOpacity();
		C.A = 1.f;
		ReticleImage->SetColorAndOpacity(C);
	}
	if (SmallReticleImage) 
	{
		FLinearColor C = SmallReticleImage->GetColorAndOpacity();
		C.A = 0.f;
		SmallReticleImage->SetColorAndOpacity(C);
	}
}

void ULockBoxWidget::PlayStartLockAnimation() 
{
	if (SmallReticleImage) 
	{
		FLinearColor C = SmallReticleImage->GetColorAndOpacity();
		C.A = 1.f;
		SmallReticleImage->SetColorAndOpacity(C);
	}
	if (LockApproachAnim) PlayAnimation(LockApproachAnim, 0.f, 1, EUMGSequencePlayMode::Forward, 1.f);
}

void ULockBoxWidget::PlayFullLockAnimation()
{
	bIsLocking = false;
	if (LockConfirm) PlayAnimation(LockConfirm, 0.f, 0, EUMGSequencePlayMode::Forward, 1.f);

	if (SmallReticleImage)
	{
		FLinearColor C = SmallReticleImage->GetColorAndOpacity();
		C.A = 0.1f;
		SmallReticleImage->SetColorAndOpacity(C);
	}

	if (ReticleImage) 
	{
		FLinearColor F = ReticleImage->GetColorAndOpacity();
		F.A = 0.1f;
		ReticleImage->SetColorAndOpacity(F);
	}
}

void ULockBoxWidget::SetReticleImage(UTexture2D* NewTexture) 
{
	if (!ReticleImage || !NewTexture) return;
	FSlateBrush Brush;
	Brush.SetResourceObject(NewTexture);
	Brush.ImageSize = FVector2D(128, 128);
	Brush.DrawAs = ESlateBrushDrawType::Image;
	ReticleImage->SetBrush(Brush);
}

void ULockBoxWidget::SetSmallerReticleImage(UTexture2D* NewTexture)
{
	if (!SmallReticleImage || !NewTexture) return;
	FSlateBrush Brush;
	Brush.SetResourceObject(NewTexture);
	Brush.ImageSize = FVector2D(128, 128);
	Brush.DrawAs = ESlateBrushDrawType::Image;
	SmallReticleImage->SetBrush(Brush);
}