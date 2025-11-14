// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/MainMenuUI/LevelButton.h"
#include "Components/ScrollBox.h"
#include "Structs and Data/LevelListData.h"
#include "FreeFlightWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLevelSelected, FName, LevelName);

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
	TSubclassOf<ULevelButton> LevelButtonClass;

	UPROPERTY(meta=(BindWidget))
	UScrollBox* LevelScrollBox;

	UFUNCTION()
	void HandleLevelButtonClicked(const FName InLevel);

private:
	UPROPERTY(EditAnywhere)
	ULevelListData* LevelList;
};
