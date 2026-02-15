// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/MainMenuUI/LevelButton.h"
#include "Components/ScrollBox.h"
#include "Structs and Data/LevelListData.h"
#include "Structs and Data/MissionData.h"
#include "FreeFlightWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLevelSelected, FMissionData, LevelName);

UCLASS()
class MYPROJECT2_API UFreeFlightWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintAssignable, Category="Events")
	FOnLevelSelected OnLevelSelected;

	UFUNCTION(BlueprintCallable)
	void InitLevels();

protected:

	UPROPERTY(EditAnywhere)
	TSubclassOf<ULevelButton> LevelButtonClass = nullptr;

	UPROPERTY(meta=(BindWidget))
	UScrollBox* LevelScrollBox = nullptr;

	UFUNCTION()
	void HandleLevelButtonClicked(FMissionData InLevel);

private:
	UPROPERTY(EditAnywhere)
	ULevelListData* LevelList = nullptr;
};
