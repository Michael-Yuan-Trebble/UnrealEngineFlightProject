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
	void SetReticleImage(UTexture2D* NewTexture);

	UFUNCTION(BlueprintCallable)
	void SetSmallerReticleImage(UTexture2D* NewTexture);

	void SelectedAnimation();

	void SelectStop();

	void UpdateLockProgress(const float Percent);

	void PlayFullLockAnimation();

	void PlayStartLockAnimation();

	void ResetLockAnimation();

	UPROPERTY(meta=(BindWidgetAnim), Transient)
	UWidgetAnimation* LockApproachAnim = nullptr;

	UPROPERTY(meta=(BindWidgetAnim), Transient)
	UWidgetAnimation* LockConfirm = nullptr;

	UPROPERTY(BlueprintReadOnly)
	AActor* TrackedTarget = nullptr;

	UPROPERTY(BlueprintReadOnly)
	bool isLockedOn = false;

	bool bIsLocking = false;

protected:
	UPROPERTY(meta = (BindWidget))
	UImage* ReticleImage = nullptr;

	UPROPERTY(meta = (BindWidget))
	UImage* SmallReticleImage = nullptr;

	UPROPERTY(meta = (BindWidget))
	USizeBox* SizeBox_Reticle = nullptr;

private:
	void ResetAnimation(UWidgetAnimation* Animation);
};
