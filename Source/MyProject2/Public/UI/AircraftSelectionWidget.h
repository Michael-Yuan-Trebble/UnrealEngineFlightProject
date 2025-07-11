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


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAircraftSelectedSignature, TSubclassOf<APawn>, SelectedAircraft);

UCLASS()
class MYPROJECT2_API UAircraftSelectionWidget : public UUserWidget
{
	GENERATED_BODY()
public:

	UAircraftSelectionWidget(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(BlueprintReadWrite)
	UAircraftDatabase* AircraftDatabase;

	UPROPERTY(BlueprintAssignable,Category = "Events")
	FAircraftSelectedSignature OnAircraftSelected;

	UFUNCTION(BlueprintCallable)
	void GetAllAircraft();

	UMenuManagerComponent* MenuManager;

	TArray<FName> Owned;

	int CurrentMoney;

protected:

	UFUNCTION()
	void HandleAircraftSelected(TSubclassOf<APawn> Aircraft);

	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> AircraftButtonClass;

	UPROPERTY(meta=(BindWidget))
	UScrollBox* AircraftScrollBox;
};
