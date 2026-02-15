// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "HitNotificationWidget.generated.h"

UCLASS()
class MYPROJECT2_API UHitNotificationWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void ShowMessage(const FText& msg);
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Message = nullptr;

	void HideMessage();

private:
	FTimerHandle HideTimerHandle;

	float DisplayDuration = 5.f;
};
