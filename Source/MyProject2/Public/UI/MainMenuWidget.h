// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "MainMenuWidget.generated.h"

class UMainMenuManager;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFreeFlightPicked);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSettingsPicked);

UCLASS()
class MYPROJECT2_API UMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UMainMenuWidget(const FObjectInitializer& ObjectInitializer);

	void CreateButtons();

	FOnSettingsPicked OnSettingsPicked;
	FOnFreeFlightPicked OnFreeFlightPicked;

	UPROPERTY(meta = (BindWidget))
	UButton* Settingsbtn;

	UPROPERTY(meta = (BindWidget))
	UButton* FreeFlightbtn;

	UPROPERTY()
	UMainMenuManager* MenuManager;

protected:
	virtual void ReleaseSlateResources(bool bReleaseChildren) override {
		Super::ReleaseSlateResources(bReleaseChildren);
		MenuManager = nullptr;
	}

private:
	void HandleSettingsbtnClicked();

	void HandleFreeFlightbtnClicked();
};
