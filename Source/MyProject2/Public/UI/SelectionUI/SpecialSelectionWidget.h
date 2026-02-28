// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Specials/BaseSpecial.h"
#include "Structs and Data/Aircraft Data/AircraftData.h"
#include "SpecialSelectionWidget.generated.h"

class UMenuManagerComponent;
class USpecialSelectionComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSpecialSelectedSignature, TSubclassOf<UBaseSpecial>, SelectedSpecial);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAdvanceWidget);

UCLASS()
class MYPROJECT2_API USpecialSelectionWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	USpecialSelectionWidget(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FSpecialSelectedSignature OnWidgetSelected;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnAdvanceWidget OnAdvance;

	UFUNCTION(BlueprintCallable)
	void GetAllSpecials();

	void Setup(UAircraftData* InAircraft, UMenuManagerComponent* InMenu, USpecialSelectionComponent* InSelect);

private:

	UPROPERTY()
	TObjectPtr<UAircraftData> AircraftSelected = nullptr;

	UPROPERTY()
	TObjectPtr<UMenuManagerComponent> MenuManager = nullptr;

	UPROPERTY()
	TObjectPtr<USpecialSelectionComponent> SpecialUI = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> Advancebtn = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> AdvanceText = nullptr;

	UFUNCTION()
	void HandleSpecialSelected(TSubclassOf<UBaseSpecial> SpecialSelected);

	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> SpecialButtonClass = nullptr;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UScrollBox> SpecialScrollBox = nullptr;

	virtual void ReleaseSlateResources(bool bReleaseChildren) override {
		Super::ReleaseSlateResources(bReleaseChildren);
		MenuManager = nullptr;
		SpecialUI = nullptr;
	}

	virtual void NativeDestruct() override {
		OnWidgetSelected.Clear();
		OnAdvance.Clear();
		Super::NativeDestruct();
	}

	UFUNCTION()
	void OnAdvancePicked();
};
