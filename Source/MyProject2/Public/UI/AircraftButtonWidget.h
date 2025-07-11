// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Structs and Data/AircraftData.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "AircraftButtonWidget.generated.h"



DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAircraftSelected, TSubclassOf<APawn>, SelectedAircraft);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAircraftPicked, UAircraftData*, PickedAircraft);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAircraftBuyPicked, FName, Name, int, Cost);

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAircraftData* ContainedData;

	int CurrentMoney;

	TArray<FName> Owned;

	void Setup(UAircraftData* AircraftData, int price, TArray<FName> Owned);

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
