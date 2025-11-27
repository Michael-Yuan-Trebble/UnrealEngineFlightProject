// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HitNotificationWidget.h"

void UHitNotificationWidget::ShowMessage(FText msg) 
{
	if (msg.IsEmpty() || !Message) return;
	Message->SetText(msg);
	SetVisibility(ESlateVisibility::Visible);
	GetWorld()->GetTimerManager().ClearTimer(HideTimerHandle);

	GetWorld()->GetTimerManager().SetTimer(
		HideTimerHandle,
		this,
		&UHitNotificationWidget::HideMessage,
		DisplayDuration,
		false
	);
}

void UHitNotificationWidget::HideMessage() 
{
	SetVisibility(ESlateVisibility::Hidden);
}