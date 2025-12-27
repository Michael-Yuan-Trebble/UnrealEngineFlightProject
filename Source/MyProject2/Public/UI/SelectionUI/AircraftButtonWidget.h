// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Structs and Data/Aircraft Data/AircraftData.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "AircraftButtonWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAircraftSelected, UAircraftData*, SelectedAircraft);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAircraftPicked, UAircraftData*, PickedAircraft);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAircraftBuyPicked, UAircraftData*, Aircraft, int, Cost);

UCLASS()
class MYPROJECT2_API UAircraftButtonWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnAircraftSelected OnAircraftSelected;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnAircraftPicked OnAircraftPicked;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnAircraftBuyPicked OnBuyCreate;

	UPROPERTY()
	UAircraftData* ContainedData;

	TArray<FName> Owned;

	void Setup(UAircraftData* AircraftData, TArray<FName> Owned);

	void AdjustButtons();

protected:

	UFUNCTION()
	void HandleButtonHover();

	UFUNCTION()
	void HandleButtonClick();

	UFUNCTION()
	void HandleBuyCreate();

	UPROPERTY(meta = (BindWidget))
	UButton* AircraftSelectButton;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* AircraftNameText;
};
