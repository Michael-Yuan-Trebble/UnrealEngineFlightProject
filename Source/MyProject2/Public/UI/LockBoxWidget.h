// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/SizeBox.h"
#include "Components/Image.h"
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
	void SetLockedOn(bool isLocked);

	UFUNCTION(BlueprintCallable)
	void SetReticleImage(UTexture2D* NewTexture);

	UFUNCTION(BlueprintCallable)
	void SetSmallerReticleImage(UTexture2D* NewTexture);

	void UpdateLockProgress(float Percent);

	void PlayFullLockAnimation();

	void PlayStartLockAnimation();

	void ResetLockAnimation();

	UPROPERTY(meta=(BindWidgetAnim), Transient)
	UWidgetAnimation* LockApproachAnim;

	UPROPERTY(meta=(BindWidgetAnim), Transient)
	UWidgetAnimation* LockConfirm;

	UPROPERTY(BlueprintReadOnly)
	AActor* TrackedTarget;

	UPROPERTY(BlueprintReadOnly)
	bool isLockedOn = false;

	bool bIsLocking = false;

protected:
	UPROPERTY(meta = (BindWidget))
	UImage* ReticleImage;

	UPROPERTY(meta = (BindWidget))
	UImage* SmallReticleImage;

	UPROPERTY(meta = (BindWidget))
	USizeBox* SizeBox_Reticle;
};
