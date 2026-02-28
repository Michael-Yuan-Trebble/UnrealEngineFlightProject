// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LockBoxWidget.generated.h"

class UImage;
class UTexture2D;

UCLASS()
class MYPROJECT2_API ULockBoxWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable)
	void SetReticleImage(UTexture2D* NewTexture);

	UFUNCTION(BlueprintCallable)
	void SetSmallerReticleImage(UTexture2D* NewTexture);

	void SelectedAnimation();

	void SelectStop();

	void UpdateLockProgress(const float Percent);

	void PlayFullLockAnimation();

	void PlayStartLockAnimation();

	void ResetLockAnimation();

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> ReticleImage = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> SmallReticleImage = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class USizeBox> SizeBox_Reticle = nullptr;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	TObjectPtr<UWidgetAnimation> LockApproachAnim = nullptr;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	TObjectPtr<UWidgetAnimation> LockConfirm = nullptr;

	UPROPERTY()
	TObjectPtr<AActor> TrackedTarget = nullptr;

	bool isLockedOn = false;

	bool bIsLocking = false;

	void ResetAnimation(UWidgetAnimation* Animation);
};
