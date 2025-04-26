// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "LockBoxWidget.generated.h"

class UImage;
class UTexture2D;

UCLASS()
class MYPROJECT2_API ULockBoxWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SetLockedOn(bool isLocked);

	UFUNCTION(BlueprintCallable)
	void SetReticleImage(UTexture2D* NewTexture);

	UPROPERTY(BlueprintReadOnly)
	AActor* TrackedTarget;

	UPROPERTY(BlueprintReadOnly)
	bool isLockedOn;

protected:
	UPROPERTY(meta = (BindWidget))
	UImage* ReticleImage;
};
