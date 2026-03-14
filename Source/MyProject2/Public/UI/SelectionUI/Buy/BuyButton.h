// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BuyButton.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBuyPressed, bool, bBought);

UCLASS()
class MYPROJECT2_API UBuyButton : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnBuyPressed OnBuyPressed;

	void Setup(const FName& Name, const int Cost);

	void TurnOffBuy();

private:

	UPROPERTY(EditAnywhere)
	FName Name = NAME_None;

	UPROPERTY(EditAnywhere)
	int Cost = 0;
	
	UFUNCTION()
	void HandleButtonClick();

	UFUNCTION()
	void HandleCancelClick();

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> AircraftBuyButton = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> CancelBuyButton = nullptr;

	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override {
		OnBuyPressed.Clear();
		Super::NativeDestruct();
	}
};
