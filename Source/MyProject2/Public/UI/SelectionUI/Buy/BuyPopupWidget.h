// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
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

	UFUNCTION(BlueprintCallable)
	void Setup(UAircraftData* AircraftData);

	void SetComp(UBuySelectionComponent* InComp) { BuyUI = InComp; };

private:

	int CurrentCurrency = 0;

	UPROPERTY()
	TObjectPtr<UAircraftData> Aircraft = nullptr;

	UPROPERTY()
	TObjectPtr<UBuySelectionComponent> BuyUI = nullptr;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> BuyButtonClass = nullptr;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class USizeBox> SizeBox = nullptr;

	virtual void ReleaseSlateResources(bool bReleaseChildren) override {
		BuyUI = nullptr;
		Super::ReleaseSlateResources(bReleaseChildren);
	}
};
