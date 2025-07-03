// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Structs and Data/AircraftData.h"
#include "AircraftSelectionWidget.generated.h"

class UAircraftButtonWidget;
class UScrollBox;
class UAircraftDatabase;

/**
 * 
 */
UCLASS()
class MYPROJECT2_API UAircraftSelectionWidget : public UUserWidget
{
	GENERATED_BODY()
public:

	UAircraftSelectionWidget(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(BlueprintReadWrite)
	UAircraftDatabase* AircraftDatabase;

	UFUNCTION(BlueprintCallable)
	void GetAllAircraft();

protected:

	UFUNCTION()
	void HandleAircraftSelected(TSubclassOf<APawn> Aircraft);

	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> AircraftButtonClass;

	UPROPERTY()
	APawn* CurrentPreview;

	UPROPERTY(meta=(BindWidget))
	UScrollBox* AircraftScrollBox;
};
