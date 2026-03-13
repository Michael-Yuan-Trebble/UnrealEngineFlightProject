// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Specials/BaseSpecial.h"
#include "Structs and Data/Aircraft Data/AircraftData.h"
#include "SpecialSelectionComponent.generated.h"

class AAircraftPlayerController;
class UMenuManagerComponent;
class UBaseSpecial;
class USpecialSelectionWidget;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYPROJECT2_API USpecialSelectionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	USpecialSelectionComponent();

	void SpecialSelectionMenu();

	void Setup(AAircraftPlayerController* InPlayer, UMenuManagerComponent* InMenu);

	UFUNCTION()
	void SetSpecial(TSubclassOf<UBaseSpecial> InSpecial);

	UFUNCTION()
	void AdvanceLevel();

	void SetAir(UAircraftData* InAir);

	void CloseAll();

	void SetWidget(const TSubclassOf<UUserWidget>& InWidget) { SelectionWidget = InWidget; };

	USpecialSelectionWidget* GetWidget() const { return SpecialSelectUI; };

private:

	UPROPERTY()
	TSubclassOf<UUserWidget> SelectionWidget = nullptr;

	UPROPERTY()
	TObjectPtr<AAircraftPlayerController> PC = nullptr;

	UPROPERTY()
	TObjectPtr<class UPlayerGameInstance> GameInstance = nullptr;

	UPROPERTY()
	TObjectPtr<USpecialSelectionWidget> SpecialSelectUI = nullptr;

	UPROPERTY()
	TObjectPtr<UMenuManagerComponent> MenuManager = nullptr;

	UPROPERTY()
	TObjectPtr<UAircraftData> SelectedAircraft = nullptr;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override 
	{
		CloseAll();
		Super::EndPlay(EndPlayReason);
	}
};
