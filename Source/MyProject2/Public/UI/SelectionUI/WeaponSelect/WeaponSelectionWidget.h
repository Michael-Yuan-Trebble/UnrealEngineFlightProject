// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Structs and Data/Aircraft Data/AircraftDatabase.h"
#include "Structs and Data/Aircraft Data/AircraftWeaponEquipInfo.h"
#include "WeaponSelectionWidget.generated.h"

class UWeaponSelectionComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FWeaponSelectedSignature, TSubclassOf<ABaseWeapon>, SelectedWeapon, FAircraftWeaponEquipInfo, Loadout);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FWeaponAddedSignature, TSubclassOf<ABaseWeapon>, SelectedWeapon, FAircraftWeaponEquipInfo, Loadout);

UCLASS()
class MYPROJECT2_API UWeaponSelectionWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UWeaponSelectionWidget(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FWeaponSelectedSignature OnWeaponSelected;

	FWeaponAddedSignature WeaponAdded;

	void GetAllAircraft();

	void SetLoadout(const FAircraftWeaponEquipInfo& In) { CurrentLoadout = In; };

	FAircraftWeaponEquipInfo GetLoadout() const { return CurrentLoadout; };

	UWeaponSelectionComponent* GetWeaponUI() const { return WeaponUI; };

	void SetWeaponUI(UWeaponSelectionComponent* InUI) { WeaponUI = InUI; };

private:

	FAircraftWeaponEquipInfo CurrentLoadout;

	UPROPERTY()
	TObjectPtr<UWeaponSelectionComponent> WeaponUI = nullptr;

	UFUNCTION()
	void HandleWeaponSelected(TSubclassOf<ABaseWeapon> Weapon);

	UFUNCTION()
	void HandleWeaponAdded(TSubclassOf<ABaseWeapon> Weapon);

	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> WeaponButtonClass = nullptr;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UScrollBox> WeaponScrollBox = nullptr;

	virtual void ReleaseSlateResources(bool bReleaseChildren) override {
		Super::ReleaseSlateResources(bReleaseChildren);
		WeaponUI = nullptr;
		WeaponScrollBox = nullptr;
	}

	virtual void NativeDestruct() override {
		OnWeaponSelected.Clear();
		Super::NativeDestruct();
	}
	void CreateButtons(const TArray<TSubclassOf<ABaseWeapon>>& Array);
};
