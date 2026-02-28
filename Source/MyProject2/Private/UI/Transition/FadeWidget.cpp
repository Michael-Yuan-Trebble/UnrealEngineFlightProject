// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Transition/FadeWidget.h"

void UFadeWidget::NativeConstruct() 
{
	Super::NativeConstruct();
	if (FadeInAnimation) {
		FWidgetAnimationDynamicEvent EndEvent;
		EndEvent.BindDynamic(this, &UFadeWidget::HandleFadeFinished);
		BindToAnimationFinished(FadeInAnimation, EndEvent);
	}
}

void UFadeWidget::HandleFadeFinished() {
	OnFadeFinished.Broadcast();
}

void UFadeWidget::PlayFadeIn() 
{
	if (FadeInAnimation) 
	{
		PlayAnimation(FadeInAnimation);
	}
}

void UFadeWidget::PlayFadeOut() 
{
	if (FadeOutAnimation) 
	{
		PlayAnimation(FadeOutAnimation);
	}
}