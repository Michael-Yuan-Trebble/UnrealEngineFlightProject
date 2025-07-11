// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/SizeBox.h"
#include "Components/Button.h"
#include "Structs and Data/AircraftData.h"
#include "BuyPopupWidget.generated.h"

class UMenuManagerComponent;

UCLASS()
class MYPROJECT2_API UBuyPopupWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UBuyPopupWidget(const FObjectInitializer& ObjectInitializer);

	UTextBlock* Question;

	UAircraftData* Aircraft;

	UFUNCTION(BlueprintCallable)
	void Setup(UAircraftData* AircraftData);

	int CurrentCurrency;

	UMenuManagerComponent* MenuManager;

protected:

	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> BuyButtonClass;

	UPROPERTY(meta=(BindWidget))
	USizeBox* SizeBox;
};
