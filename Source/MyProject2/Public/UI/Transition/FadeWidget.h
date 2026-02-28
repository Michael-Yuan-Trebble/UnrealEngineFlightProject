// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FadeWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FFadeInFinishedSignature);

UCLASS()
class MYPROJECT2_API UFadeWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	FFadeInFinishedSignature OnFadeFinished;

	virtual void NativeConstruct() override;

	UFUNCTION()
	void HandleFadeFinished();

	UFUNCTION(BlueprintCallable)
	void PlayFadeIn();

	UFUNCTION(BlueprintCallable)
	void PlayFadeOut();

protected:
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	TObjectPtr<UWidgetAnimation> FadeInAnimation = nullptr;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	TObjectPtr<UWidgetAnimation> FadeOutAnimation = nullptr;
};
