// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Structs and Data/Aircraft Data/AircraftData.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "WeaponButtonWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponSelected, TSubclassOf<ABaseWeapon>, SelectedAircraft);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponPicked, TSubclassOf<ABaseWeapon>, PickedAircraft);

UCLASS()
class MYPROJECT2_API UWeaponButtonWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnWeaponSelected OnWeaponSelected;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnWeaponPicked OnWeaponPicked;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<ABaseWeapon> ButtonWeapon = nullptr;

	void SetupWeapons(const TSubclassOf<ABaseWeapon>& WeaponData);

protected:

	UFUNCTION()
	void HandleButtonHover();

	UFUNCTION()
	void HandleButtonClick();

	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UButton* WeaponSelectButton = nullptr;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* WeaponNameText = nullptr;

	virtual void NativeDestruct() override {
		OnWeaponSelected.Clear();
		OnWeaponPicked.Clear();
		Super::NativeDestruct();
	}
};
