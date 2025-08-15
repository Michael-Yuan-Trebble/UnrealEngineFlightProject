// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Structs and Data/AircraftData.h"
#include "AircraftSelectionWidget.generated.h"

class UAircraftButtonWidget;
class UScrollBox;
class UMenuManagerComponent;
class UAircraftDatabase;
class UAircraftSelectionComponent;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAircraftSelectedSignature, UAircraftData*, SelectedAircraft);

UCLASS()
class MYPROJECT2_API UAircraftSelectionWidget : public UUserWidget
{
	GENERATED_BODY()
public:

	UAircraftSelectionWidget(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(BlueprintReadWrite)
	UAircraftDatabase* AircraftDatabase;

	UPROPERTY(BlueprintAssignable,Category = "Events")
	FAircraftSelectedSignature OnWidgetSelected;

	UFUNCTION(BlueprintCallable)
	void GetAllAircraft();

	void Setup(UAircraftDatabase* Database, TArray<FName> InOwn, UMenuManagerComponent* InMenu, UAircraftSelectionComponent* InSelect);

	UPROPERTY()
	UMenuManagerComponent* MenuManager;

	UPROPERTY()
	UAircraftSelectionComponent* AircraftUI;

	TArray<FName> Owned;

	TMap<FName, UAircraftButtonWidget*> ButtonArray;

	int CurrentMoney;

	void UpdateAircraft(FName AircraftChange);

protected:

	UFUNCTION()
	void HandleAircraftSelected(UAircraftData* Aircraft);

	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> AircraftButtonClass;

	UPROPERTY(meta=(BindWidget))
	UScrollBox* AircraftScrollBox;

	virtual void ReleaseSlateResources(bool bReleaseChildren) override {
		Super::ReleaseSlateResources(bReleaseChildren);
		MenuManager = nullptr;
		AircraftUI = nullptr;
		AircraftScrollBox = nullptr;
	}
};
