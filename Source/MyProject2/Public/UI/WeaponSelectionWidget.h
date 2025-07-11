// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Structs and Data/AircraftDatabase.h"
#include "WeaponSelectionWidget.generated.h"

class UWeaponButtonWidget;
class UScrollBox;
class UMenuManagerComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWeaponSelectedSignature, TSubclassOf<ABaseWeapon>, SelectedWeapon);

UCLASS()
class MYPROJECT2_API UWeaponSelectionWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UWeaponSelectionWidget(const FObjectInitializer& ObjectInitializer);

	FPylonLoadout* CurrentLoadout;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FWeaponSelectedSignature OnWeaponSelected;

	UFUNCTION(BlueprintCallable)
	void GetAllAircraft();

	UMenuManagerComponent* MenuManager;

protected:
	UFUNCTION()
	void HandleWeaponSelected(TSubclassOf<ABaseWeapon> Weapon);

	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> WeaponButtonClass;

	UPROPERTY()
	AActor* CurrentPreview;

	UPROPERTY(meta=(BindWidget))
	UScrollBox* WeaponScrollBox;

private:
	void CreateButtons(TArray<TSubclassOf<ABaseWeapon>> Array);
};
