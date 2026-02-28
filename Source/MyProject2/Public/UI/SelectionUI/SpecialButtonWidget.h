// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SpecialButtonWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSpecialPicked, TSubclassOf<UBaseSpecial>, SelectedSpecial);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSpecialSelected);

class UBaseSpecial;

UCLASS()
class MYPROJECT2_API USpecialButtonWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnSpecialPicked OnSpecialPicked;

	void Setup(const TSubclassOf<UBaseSpecial>& SpecialData);

private:

	UPROPERTY()
	TSubclassOf<UBaseSpecial> ContainedData = nullptr;

	UFUNCTION()
	void HandleButtonClick();

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> SpecialSelectButton = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> SpecialNameText = nullptr;

	virtual void NativeDestruct() override {
		OnSpecialPicked.Clear();
		Super::NativeDestruct();
	}
};
