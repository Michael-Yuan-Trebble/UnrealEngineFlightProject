// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/SizeBox.h"
#include "Components/Button.h"
#include "Structs and Data/Aircraft Data/AircraftData.h"
#include "BuyPopupWidget.generated.h"

class UMenuManagerComponent;
class UBuySelectionComponent;

UCLASS()
class MYPROJECT2_API UBuyPopupWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UBuyPopupWidget(const FObjectInitializer& ObjectInitializer);

	UPROPERTY()
	UAircraftData* Aircraft;

	UFUNCTION(BlueprintCallable)
	void Setup(UAircraftData* AircraftData);

	int CurrentCurrency;

	UPROPERTY()
	UBuySelectionComponent* BuyUI;

protected:

	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> BuyButtonClass;

	UPROPERTY(meta=(BindWidget))
	USizeBox* SizeBox;

	virtual void ReleaseSlateResources(bool bReleaseChildren) override {
		BuyUI = nullptr;
		Super::ReleaseSlateResources(bReleaseChildren);
	}
};
