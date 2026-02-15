// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Specials/BaseSpecial.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "SpecialButtonWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSpecialPicked, TSubclassOf<UBaseSpecial>, SelectedSpecial);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSpecialSelected);

UCLASS()
class MYPROJECT2_API USpecialButtonWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnSpecialPicked OnSpecialPicked;

	UPROPERTY()
	TSubclassOf<UBaseSpecial> ContainedData = nullptr;

	void Setup(const TSubclassOf<UBaseSpecial>& SpecialData);

protected:
	UFUNCTION()
	void HandleButtonClick();

	UPROPERTY(meta = (BindWidget))
	UButton* SpecialSelectButton = nullptr;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* SpecialNameText = nullptr;

	virtual void NativeDestruct() override {
		OnSpecialPicked.Clear();
		Super::NativeDestruct();
	}
};
