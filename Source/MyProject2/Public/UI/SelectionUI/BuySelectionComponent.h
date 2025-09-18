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
	AAircraftPlayerController* PC;

	UPROPERTY()
	UPlayerGameInstance* GI;
	
	UPROPERTY()
	UAircraftData* Aircraft;

	UPROPERTY()
	UAircraftSelectionComponent* AircraftUI;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UUserWidget> BuyPopupClass;

	UPROPERTY()
	UBuyPopupWidget* BuyWidget;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> GreyOutClass;

	UPROPERTY()
	UUserWidget* GreyOut;

protected:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override {
		CloseAll();
		Super::EndPlay(EndPlayReason);
	}
};
