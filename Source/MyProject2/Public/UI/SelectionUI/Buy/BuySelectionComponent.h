// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Structs and Data/Aircraft Data/AircraftData.h"
#include "BuySelectionComponent.generated.h"

class UAircraftSelectionComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYPROJECT2_API UBuySelectionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UBuySelectionComponent();
	
	void BuyPopupMenu();

	UFUNCTION()
	void BuyAircraft(bool bBought);

	void AddAircraft(UAircraftData* Data, UAircraftSelectionComponent* InUI);

	void CloseAll();

	void SetWidget(const TSubclassOf<UUserWidget>& InWidget) { BuyPopupClass = InWidget; };

	void SetGrey(const TSubclassOf<UUserWidget>& InWidget) { GreyOutClass = InWidget; };

private:

	UPROPERTY()
	TObjectPtr<UAircraftData> Aircraft = nullptr;

	UPROPERTY()
	TObjectPtr<UAircraftSelectionComponent> AircraftUI = nullptr;

	UPROPERTY()
	TSubclassOf<UUserWidget> BuyPopupClass = nullptr;

	UPROPERTY()
	class UBuyPopupWidget* BuyWidget = nullptr;

	UPROPERTY()
	TSubclassOf<UUserWidget> GreyOutClass = nullptr;

	UPROPERTY()
	TObjectPtr<UUserWidget> GreyOut = nullptr;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override {
		CloseAll();
		Super::EndPlay(EndPlayReason);
	}
};
