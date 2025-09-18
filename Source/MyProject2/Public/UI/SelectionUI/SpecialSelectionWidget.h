// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Specials/BaseSpecial.h"
#include "Structs and Data/Aircraft Data/AircraftData.h"
#include "Components/ScrollBox.h"
#include "SpecialSelectionWidget.generated.h"

class UMenuManagerComponent;
class USpecialSelectionComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSpecialSelectedSignature, TSubclassOf<UBaseSpecial>, SelectedSpecial);

UCLASS()
class MYPROJECT2_API USpecialSelectionWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	USpecialSelectionWidget(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FSpecialSelectedSignature OnWidgetSelected;

	UPROPERTY()
	UAircraftData* AircraftSelected;

	UFUNCTION(BlueprintCallable)
	void GetAllSpecials();

	void Setup(UAircraftData* InAircraft, UMenuManagerComponent* InMenu, USpecialSelectionComponent* InSelect);

	UPROPERTY()
	UMenuManagerComponent* MenuManager;

	UPROPERTY()
	USpecialSelectionComponent* SpecialUI;

protected:
	UFUNCTION()
	void HandleSpecialSelected(TSubclassOf<UBaseSpecial> SpecialSelected);

	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> SpecialButtonClass;

	UPROPERTY(meta=(BindWidget))
	UScrollBox* SpecialScrollBox;

	virtual void ReleaseSlateResources(bool bReleaseChildren) override {
		Super::ReleaseSlateResources(bReleaseChildren);
		MenuManager = nullptr;
		SpecialUI = nullptr;
	}
};
