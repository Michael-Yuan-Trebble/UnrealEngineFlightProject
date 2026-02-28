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

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	void InitializeBounds(const FVector2D& InWorldMin, const FVector2D& InWorldMax);

private:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> MapImage = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> PlayerIcon = nullptr;

	FVector2D WorldMin = FVector2D::ZeroVector;
	FVector2D WorldMax = FVector2D::ZeroVector;

	FVector2D WorldToMap(const FVector& WorldLocation) const;
};
