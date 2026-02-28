// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Structs and Data/Aircraft Data/AircraftData.h"
#include "BuySelectionComponent.generated.h"

class AAircraftPlayerController;
class UPlayerGameInstance;
class UAircraftSelectionComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYPROJECT2_API UBuySelectionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UBuySelectionComponent();
	
	void BuyPopupMenu();

	void Setup(AAircraftPlayerController* InPlayer, UPlayerGameInstance* InGI);

	UFUNCTION()
	void BuyAircraft(FName Name, int Cost);

	UFUNCTION()
	void CancelBuy();

	void AddAircraft(UAircraftData* Data, UAircraftSelectionComponent* InUI);

	void CloseAll();

	void SetWidget(const TSubclassOf<UUserWidget>& InWidget) { BuyPopupClass = InWidget; };

private:

	UPROPERTY()
	TObjectPtr<AAircraftPlayerController> PC = nullptr;

	UPROPERTY()
	TObjectPtr<UPlayerGameInstance> GI = nullptr;

	UPROPERTY()
	TObjectPtr<UAircraftData> Aircraft = nullptr;

	UPROPERTY()
	TObjectPtr<UAircraftSelectionComponent> AircraftUI = nullptr;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> BuyPopupClass = nullptr;

	UPROPERTY()
	class UBuyPopupWidget* BuyWidget = nullptr;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> GreyOutClass = nullptr;

	UPROPERTY()
	TObjectPtr<UUserWidget> GreyOut = nullptr;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override {
		CloseAll();
		Super::EndPlay(EndPlayReason);
	}
};
