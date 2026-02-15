// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Structs and Data/Aircraft Data/AircraftDatabase.h"
#include "WeaponSelectionWidget.generated.h"

class UWeaponButtonWidget;
class UScrollBox;
class UWeaponSelectionComponent;
class UMenuManagerComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWeaponSelectedSignature, TSubclassOf<ABaseWeapon>, SelectedWeapon);

UCLASS()
class MYPROJECT2_API UWeaponSelectionWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UWeaponSelectionWidget(const FObjectInitializer& ObjectInitializer);

	FPylonLoadout* CurrentLoadout = nullptr;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FWeaponSelectedSignature OnWeaponSelected;

	void GetAllAircraft();
	
	UPROPERTY()
	UWeaponSelectionComponent* WeaponUI = nullptr;

protected:
	UFUNCTION()
	void HandleWeaponSelected(TSubclassOf<ABaseWeapon> Weapon);

	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> WeaponButtonClass = nullptr;

	UPROPERTY(meta=(BindWidget))
	UScrollBox* WeaponScrollBox = nullptr;

	virtual void ReleaseSlateResources(bool bReleaseChildren) override {
		Super::ReleaseSlateResources(bReleaseChildren);
		WeaponUI = nullptr;
		WeaponScrollBox = nullptr;
	}

	virtual void NativeDestruct() override {
		OnWeaponSelected.Clear();
		Super::NativeDestruct();
	}

private:
	void CreateButtons(const TArray<TSubclassOf<ABaseWeapon>>& Array);
};
