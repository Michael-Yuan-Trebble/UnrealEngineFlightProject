// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Structs and Data/Aircraft Data/AircraftData.h"
#include "AircraftSelectionWidget.generated.h"

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

	UPROPERTY(BlueprintAssignable,Category = "Events")
	FAircraftSelectedSignature OnWidgetSelected;

	UFUNCTION(BlueprintCallable)
	void GetAllAircraft();

	void Setup(UAircraftDatabase* Database, 
		TArray<FName> InOwn, 
		UMenuManagerComponent* InMenu, 
		UAircraftSelectionComponent* InSelect);

	void UpdateAircraft(const FName& AircraftChange);

private:

	UPROPERTY()
	TObjectPtr<UMenuManagerComponent> MenuManager = nullptr;

	UPROPERTY()
	TObjectPtr<UAircraftDatabase> AircraftDatabase = nullptr;

	UPROPERTY()
	TObjectPtr<UAircraftSelectionComponent> AircraftUI = nullptr;

	TArray<FName> Owned;

	UPROPERTY()
	TMap<FName, class UAircraftButtonWidget*> ButtonArray;

	UFUNCTION()
	void HandleAircraftSelected(UAircraftData* Aircraft);

	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> AircraftButtonClass = nullptr;

	UPROPERTY(meta=(BindWidget))
	class UScrollBox* AircraftScrollBox = nullptr;

	virtual void ReleaseSlateResources(bool bReleaseChildren) override {
		MenuManager = nullptr;
		AircraftUI = nullptr;
		AircraftScrollBox = nullptr;
		Super::ReleaseSlateResources(bReleaseChildren);
	}

	virtual void NativeDestruct() override {
		OnWidgetSelected.Clear();
		Super::NativeDestruct();
	}
};
