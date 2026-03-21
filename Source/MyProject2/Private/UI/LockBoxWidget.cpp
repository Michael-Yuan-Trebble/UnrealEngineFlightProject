// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/LockBoxWidget.h"
#include "Engine/Texture2D.h"
#include "Components/SizeBox.h"
#include "Styling/SlateBrush.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Blueprint/UserWidget.h"
#include "Animation/UMGSequencePlayer.h"
#include "Animation/WidgetAnimation.h"
#include "Debug/DebugHelper.h"

void ULockBoxWidget::NativeConstruct() 
{
	Super::NativeConstruct();

	HideSmallReticle();
	if (IsValid(UnitNameTextBox)) UnitNameTextBox->SetText(FText::GetEmpty());
}

void ULockBoxWidget::UpdateLockProgress(const float Percent) 
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
			bIsLocking = false;
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
	if (LockApproachAnim)
		ResetAnimation(LockApproachAnim);

	if (LockConfirm) 
		ResetAnimation(LockConfirm);

	if (IsValid(ReticleImage))
	{
		FLinearColor C = ReticleImage->GetColorAndOpacity();
		C.A = 1.f;
		C.G = 1.f;
		C.R = 0.f;
		ReticleImage->SetColorAndOpacity(C);
	}
	if (IsValid(SmallReticleImage))
	{
		FLinearColor C = SmallReticleImage->GetColorAndOpacity();
		C.A = 0.f;
		C.G = 1.f;
		C.R = 0.f;
		SmallReticleImage->SetColorAndOpacity(C);
	}
}

void ULockBoxWidget::PlayStartLockAnimation() 
{
	if (IsValid(SmallReticleImage))
	{
		FLinearColor C = SmallReticleImage->GetColorAndOpacity();
		C.A = 1.f;
		SmallReticleImage->SetColorAndOpacity(C);
	}
	if (LockApproachAnim) PlayAnimation(LockApproachAnim, 0.f, 1, EUMGSequencePlayMode::Forward, 1.f);
}

void ULockBoxWidget::PlayFullLockAnimation()
{
	if (LockConfirm)
		PlayAnimation(LockConfirm, LockConfirm->GetEndTime(), 1, EUMGSequencePlayMode::Forward, 0.f);

	if (LockApproachAnim)
		PlayAnimation(LockApproachAnim, LockApproachAnim->GetEndTime(), 1, EUMGSequencePlayMode::Forward, 0.f);
}

void ULockBoxWidget::SelectedAnimation(const FName& TargetName)
{
	if (LockConfirm) PlayAnimation(LockConfirm, 0.f, 0.f, EUMGSequencePlayMode::Forward, 1.f);
	if (IsValid(UnitNameTextBox)) UnitNameTextBox->SetText(FText::FromName(TargetName));
}

void ULockBoxWidget::SelectStop() 
{
	StopAnimation(LockConfirm);
	HideSmallReticle();
	if (IsValid(UnitNameTextBox)) UnitNameTextBox->SetText(FText::GetEmpty());
}

void ULockBoxWidget::SetReticleImage(UTexture2D* NewTexture) 
{
	if (!IsValid(ReticleImage) || !IsValid(NewTexture)) return;
	FSlateBrush Brush;
	Brush.SetResourceObject(NewTexture);
	Brush.ImageSize = FVector2D(128, 128);
	Brush.DrawAs = ESlateBrushDrawType::Image;
	ReticleImage->SetBrush(Brush);
}

void ULockBoxWidget::SetSmallerReticleImage(UTexture2D* NewTexture)
{
	if (!IsValid(SmallReticleImage) || !IsValid(NewTexture)) return;
	FSlateBrush Brush;
	Brush.SetResourceObject(NewTexture);
	Brush.ImageSize = FVector2D(128, 128);
	Brush.DrawAs = ESlateBrushDrawType::Image;
	SmallReticleImage->SetBrush(Brush);
}

void ULockBoxWidget::ResetAnimation(UWidgetAnimation* Animation) 
{
	if (!Animation) return;
	StopAnimation(Animation);
	SetAnimationCurrentTime(Animation, 0.f);
}

void ULockBoxWidget::HideSmallReticle() {
	if (!SmallReticleImage) return;
	FLinearColor Color = SmallReticleImage->GetColorAndOpacity();
	Color.A = 0.f;
	SmallReticleImage->SetColorAndOpacity(Color);
}