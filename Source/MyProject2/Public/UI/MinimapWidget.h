// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MinimapWidget.generated.h"

class UImage;

UCLASS()
class MYPROJECT2_API UMinimapWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta=(BindWidget))
	UImage* MapImage;

	UPROPERTY(meta=(BindWidget))
	UImage* PlayerIcon;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	void InitializeBounds(const FVector2D& InWorldMin, const FVector2D& InWorldMax);

private:
	FVector2D WorldMin;
	FVector2D WorldMax;

	FVector2D WorldToMap(const FVector& WorldLocation) const;
};
