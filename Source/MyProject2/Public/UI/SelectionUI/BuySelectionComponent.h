// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Structs and Data/Aircraft Data/AircraftData.h"
#include "BuySelectionComponent.generated.h"

class AAircraftPlayerController;
class AAircraftSelectionGamemode;
class UPlayerGameInstance;
class UMenuManagerComponent;
class UBuyPopupWidget;
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

	UPROPERTY()
	AAircraftPlayerController* PC = nullptr;

	UPROPERTY()
	UPlayerGameInstance* GI = nullptr;
	
	UPROPERTY()
	UAircraftData* Aircraft = nullptr;

	UPROPERTY()
	UAircraftSelectionComponent* AircraftUI = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UUserWidget> BuyPopupClass = nullptr;

	UPROPERTY()
	UBuyPopupWidget* BuyWidget = nullptr;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> GreyOutClass = nullptr;

	UPROPERTY()
	UUserWidget* GreyOut = nullptr;

protected:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override {
		CloseAll();
		Super::EndPlay(EndPlayReason);
	}
};
