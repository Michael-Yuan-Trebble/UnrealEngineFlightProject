// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "BuyButton.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBuyPicked, FName, PurchasedAircraft, int, cost);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCancelPicked);

UCLASS()
class MYPROJECT2_API UBuyButton : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnBuyPicked OnBuyPicked;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnCancelPicked OnCancelPicked;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Cost;
	
	void Setup(FName Name, int Cost);

protected:
	
	UFUNCTION()
	void HandleButtonClick();

	UFUNCTION()
	void HandleCancelClick();

	UPROPERTY(meta = (BindWidget))
	UButton* AircraftBuyButton;

	UPROPERTY(meta = (BindWidget))
	UButton* CancelBuyButton;

	virtual void NativeConstruct() override;
};
