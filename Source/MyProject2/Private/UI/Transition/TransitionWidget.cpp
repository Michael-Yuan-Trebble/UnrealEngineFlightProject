// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Transition/TransitionWidget.h"
#include "Player Info/PlayerGameInstance.h"

void UTransitionWidget::NativeConstruct() {
	Super::NativeConstruct();
	SetIsFocusable(true);
}

void UTransitionWidget::StartScreen() 
{
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC) return;
	PC->DisableInput(PC);
	UPlayerGameInstance* GI = GetWorld()->GetGameInstance<UPlayerGameInstance>();
	if (!IsValid(GI)) return;
	GI->FadeIn();
	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle,
		this,
		&UTransitionWidget::EnableInput,
		4.0f,
		false
	);
}

void UTransitionWidget::OnLevelAdvance() {
}

FReply UTransitionWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) {
	if (bInputEnabled && !bAdvancing) 
	{
		bAdvancing = true;
		OnLevelAdvance();
		return FReply::Handled();
	}
	return FReply::Unhandled();
}

FReply UTransitionWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) {
	if (bInputEnabled && !bAdvancing) {
		bAdvancing = true;
		OnLevelAdvance();
		return FReply::Handled();
	}
	return FReply::Unhandled();
}

void UTransitionWidget::HideScreen() {

}

void UTransitionWidget::EnableInput() {
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC) return;
	PC->EnableInput(PC);
	bInputEnabled = true;
}

void UTransitionWidget::DisableAllUI() {
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC) return;
	PC->SetInputMode(FInputModeUIOnly());
	PC->bShowMouseCursor = false;
	PC->SetIgnoreMoveInput(true);
	PC->SetIgnoreLookInput(true);
}