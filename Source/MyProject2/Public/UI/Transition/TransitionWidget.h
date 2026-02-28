// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TransitionWidget.generated.h"

UCLASS()
class MYPROJECT2_API UTransitionWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void StartScreen();

	void OnLevelAdvance();

	void HideScreen();

	void DisableAllUI();

	UFUNCTION()
	void EnableInput();

private:

	virtual void NativeConstruct() override;

	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	FTimerHandle TimerHandle;

	bool bInputEnabled = false;

	bool bAdvancing = false;
};
