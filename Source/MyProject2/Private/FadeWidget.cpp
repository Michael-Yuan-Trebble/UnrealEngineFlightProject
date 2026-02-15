// Fill out your copyright notice in the Description page of Project Settings.


#include "FadeWidget.h"

void UFadeWidget::NativeConstruct() 
{
	Super::NativeConstruct();
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