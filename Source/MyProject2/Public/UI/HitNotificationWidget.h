// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HitNotificationWidget.generated.h"

UCLASS()
class MYPROJECT2_API UHitNotificationWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void ShowMessage(const FText& msg);

	void HideMessage();

private:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> Message = nullptr;

	FTimerHandle HideTimerHandle;

	float DisplayDuration = 5.f;
};
