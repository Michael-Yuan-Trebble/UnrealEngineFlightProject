// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Structs and Data/Aircraft Data/AircraftData.h"
#include "Components/TextBlock.h"
#include "AircraftPriceDisplayWidget.generated.h"

UCLASS()
class MYPROJECT2_API UAircraftPriceDisplayWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void DisplayAircraftPriceInfo(UAircraftData* Aircraft);

protected:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* AircraftPriceText = nullptr;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* AircraftNameText = nullptr;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* AircraftDescriptionText = nullptr;
};
